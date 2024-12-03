#include "neural-net/Network.h"
#include "neural-net/network_functions.h"
#include "shared/arr_helpers.h"
#include "train/training_data.h"
#include "train/training_data_loader.h"
#include "train/training_functions.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <err.h>

// defining constants
#define INPUT_SIZE 28 * 28
#define LAYER_NUMBER 3
#define LAYER_SIZES {32, 32, 26}
#define DATA_NB 372038
#define MINIBATCH_SIZE 300
#define EPOCHS 10
#define RATE 1e-1
#define BACKPROP_NUMBER 100
#define DEFAULT_SAVE_PATH "./models/letter.model"
// fork specific
#define NETWORK_NUMBER 8
// thread specific
#define THREAD_NUMBER 1

void letter_train(struct training_set set, char* save_path) {
  printf(" -- letter_train - save path: %s -- \n", save_path);
  // creating network and filling it
  printf("------------\n");
  printf("Neural network :\n");
  printf("------------\n");
  size_t layersizes[LAYER_NUMBER] = LAYER_SIZES;
  struct Network net = {INPUT_SIZE, LAYER_NUMBER, layersizes, NULL};
  printf(
      "Creating and filling network with random initial weights and biases\n");
  fill_network(&net);
  // training the network
  printf("--------------------\n");
  printf("Training the network\n");
  train(&net, set, RATE, MINIBATCH_SIZE, EPOCHS, save_path, BACKPROP_NUMBER, THREAD_NUMBER, 1);
  printf("Finished training the network\n");
  printf("--------------------\n");
  // saving it
  printf("Saving the model\n");
  save_network(save_path, net);
  // cleanup
  printf("Freeing the heap\n");
  free_network(&net);
  free_training_set(set);
  printf("------------\n");
  printf("Finished\n");
}


void letter_train_existing(struct training_set set, char* model_path, char* save_path) {
  printf(" -- letter_train_existing - model path: %s,  save path: %s -- \n", model_path, save_path);
  // creating network and filling it
  printf("------------\n");
  printf("Neural network :\n");
  printf("------------\n");
  struct Network net = load_network(model_path);
  // training the network
  printf("--------------------\n");
  printf("Training the network\n");
  train(&net, set, RATE, MINIBATCH_SIZE, EPOCHS, save_path, BACKPROP_NUMBER, THREAD_NUMBER, 1);
  printf("Finished training the network\n");
  printf("--------------------\n");
  // saving it
  printf("Saving the model\n");
  save_network(save_path, net);
  // cleanup
  printf("Freeing the heap\n");
  free_network(&net);
  free_training_set(set);
  printf("------------\n");
  printf("Finished\n");
}

void letter_train_fork(struct training_set set, char* save_path){
  printf(" -- letter_train_fork - save path: %s -- \n", save_path);
  // creating model network and filling it
  printf("------------\n");
  printf("Neural network :\n");
  size_t layersizes[LAYER_NUMBER] = LAYER_SIZES;
  struct Network net = {INPUT_SIZE, LAYER_NUMBER, layersizes, NULL};
  // training the network
  printf("Training the networks\n");
  printf("------------\n");
  train_fork(net, set, RATE, MINIBATCH_SIZE, EPOCHS, BACKPROP_NUMBER, 
             NETWORK_NUMBER, THREAD_NUMBER, save_path);
  printf("------------\n");
  printf("Finished training the network\n");
  printf("--------------------\n");
  // saving it
  // cleanup
  printf("-> Freeing the heap\n");
  free_network(&net);
  free_training_set(set);
  printf("-> Finished\n");
}

void use_model(char* model_path, char* image_path){
  struct Network net = load_network(model_path);
  double* input = image_to_input(image_path);
  double **a_mat = calloc(net.layernb, sizeof(double*));
  double **z_mat = calloc(net.layernb, sizeof(double*));
  double* res = feedforward(net, input, z_mat, a_mat);
  free(a_mat);
  free(z_mat);
  print_double_arr(res, 26);
  printf("Network's prediction : %c\n", output_to_prediction(res));
  free(res);
}

void use_model_random(char *model_path, struct training_set set){
  long i;
  struct Network net = load_network(model_path);
  while (1){
    i = rand() % set.data_number;
    struct training_data data = *(set.data+i);
    double **a_mat = calloc(net.layernb, sizeof(double*));
    double **z_mat = calloc(net.layernb, sizeof(double*));
    double* res = feedforward(net, data.inputs, z_mat, a_mat);
    free(a_mat);
    free(z_mat);
    printf("%lu - Expected guess : %c\n", i, output_to_prediction(data.expected_output));
    print_double_arr(data.expected_output, 26);
    printf("%lu - Network guessed : %c\n", i, output_to_prediction(res));
    print_double_arr(res, 26);
  }
}

void test_dataset(struct training_set set){
  long i;
  while (1){
    i = rand() % set.data_number;
    printf("%lu - Expected guess : %c\n", i, output_to_prediction((set.data+i)->expected_output));
    print_double_arr((set.data+i)->expected_output, 26);
    input_to_image((set.data+i)->inputs);
  }
}

void test_image(char* path){
  double *i = image_to_input(path);
  input_to_image(i);
}

void test_derivative(){
  // derivative should be -0.160411
  double inputs1[1] = {0.5};
  double expected1[1] = {1};
  struct training_data d = {1, inputs1, expected1};
  size_t layersizes1[1] = {1};
  struct Network net1 = {1, 1, layersizes1, NULL};
  fill_network(&net1);
  *(net1.layers->neurons->weights) = 0.5;
  (net1.layers->neurons->bias) = 0.5;
  (net1.layers->neurons->inputsize) = 1;
  //printf("%f\n", true_derivative_weight(net1, (net1.layers->neurons), 0, d));
  double *dw = calloc(1, sizeof(double));
  double *db = calloc(1, sizeof(double));
  back_propagate(&net1, d, dw, db);
}

void test_cost_function(){
  double p[9] = {0,0,0,0,1,1,0,0,0};
  double y[9] = {0,0,1,0,0,0,0,0,0};
  test_cost(p, y);
  double p1[9] = {0,0,0.1,0,0,0,0,0,0};
  double y1[9] = {0,0,1,0,0,0,0,0,0};
  test_cost(p1, y1);
  double p2[9] = {0,0,0.9,0,0,0,0,0,0};
  double y2[9] = {0,0,1,0,0,0,0,0,0};
  test_cost(p2, y2);
}

int main(int argc, char* argv[]) {
  test_derivative();
  // handling arguments
  char* opt = NULL;
  char* existing_path = NULL;
  char* save_opt = NULL;
  char* save_path = NULL;
  char* image_path = NULL;
  if (argc > 1){
    opt = *(argv+1);
    if (strcmp(opt, "-e") == 0){
      if (argc == 2){
        errx(EXIT_FAILURE, "Incorrect usage of option -e\n\
          Should be \"-e {MODEL_NAME}\"\n");
      }
      existing_path = *(argv+2);
      if (argc > 3){
        save_opt= *(argv+3);
        if (argc == 4){
          errx(EXIT_FAILURE, "Incorrect usage of option -s\n\
            Should be \"-s {SAVE_MODEL_NAME}\"\n");
        }
        save_path = *(argv+4);
      }
      if (argc > 5){
        errx(EXIT_FAILURE, "Too many arguments\n");
      }
    }
    else if (strcmp(opt, "-f") == 0){
      if (argc > 2){
        save_opt= *(argv+2);
        if (argc == 3){
          errx(EXIT_FAILURE, "Incorrect usage of option -s\n\
            Should be \"-s {SAVE_MODEL_NAME}\"\n");
        }
        save_path = *(argv+3);
      }
      if (argc > 4){
        errx(EXIT_FAILURE, "Too many arguments\n");
      }
    }
    else if (strcmp(opt, "-s") == 0){
      save_opt = opt;
      opt = NULL;
      if (argc == 2){
        errx(EXIT_FAILURE, "Incorrect usage of option -s\n\
        Should be \"-s {SAVE_MODEL_NAME}\"\n");
      }
      save_path = *(argv+2);
    }
    else if (strcmp(opt, "-u") == 0){
      if (argc != 4){
        errx(EXIT_FAILURE, "Incorrect usage of option -u\n\
             Should be \"-u {MODEL_NAME} {IMAGE_PATH}\"\n");
      }
      existing_path = *(argv+2);
      image_path = *(argv+3);
    }
    else if (strcmp(opt, "-t") == 0){
      if (argc != 2){
        errx(EXIT_FAILURE, "Too many arguments\n");
      }
    }
    else if (strcmp(opt, "--test-image") == 0){
      if (argc != 3){
        errx(EXIT_FAILURE, "Incorrect usage of option -u\n\
             Should be \"--test-image {IMAGE_PATH}\"\n");
      }
      else{
        image_path = *(argv+2);
      }
    }
    else if (strcmp(opt, "--use-random") == 0){
      if (argc != 3){
        errx(EXIT_FAILURE, "Incorrect usage of option -u\n\
             Should be \"--use-random {MODEL_PATH}\"\n");
      }
      else{
        existing_path = *(argv+2);
      }
    }
    else{
      errx(EXIT_FAILURE, "Unknown argument \"%s\"\n", opt);
    }
  }
  if (opt != NULL && strcmp(opt, "-u") == 0){
    use_model(existing_path, image_path);
    exit(EXIT_SUCCESS);
  }
  else if (opt != NULL && strcmp(opt, "--test-image") == 0){
    test_image(image_path);
    exit(EXIT_SUCCESS);
  }

  // creating training set
  printf("------------\n");
  printf("Training set :\n");
  printf("------------\n");
  double **inputs;
  double **outputs;
  FILE *fptr;
  fptr = fopen("./training-set/huge_data.csv", "r");
  if (fptr == NULL) {
    printf("File could not be opened");
    exit(EXIT_FAILURE);
  }
  printf("Loading training data...\n");
  load_training_data(fptr, &inputs, &outputs, DATA_NB, INPUT_SIZE);
  fclose(fptr);
  printf("Creating training set...\n");
  struct training_set letter_training_set = 
    create_training_set(inputs, outputs, DATA_NB, INPUT_SIZE);
  free(inputs);
  free(outputs);
  // launch one of the functions
  if (opt == NULL){
    if (save_opt == NULL){
      letter_train(letter_training_set, DEFAULT_SAVE_PATH);
    }
    else{
      letter_train(letter_training_set, save_path);
    }
  }
  else if (strcmp(opt, "-f") == 0){
    if (save_opt == NULL){
      letter_train_fork(letter_training_set, DEFAULT_SAVE_PATH);
    }
    else{
      letter_train_fork(letter_training_set, save_path);
    }
  }
  else if (strcmp(opt, "-e") == 0){
    if (save_opt == NULL){
      letter_train_existing(letter_training_set, existing_path, DEFAULT_SAVE_PATH);
    }
    else{
      letter_train_existing(letter_training_set, existing_path, save_path);
    }
  }
  else if (strcmp(opt, "-t") == 0){
    test_dataset(letter_training_set);
  }
  else if (strcmp(opt, "--use-random") == 0){
    use_model_random(existing_path, letter_training_set);
  }
}
