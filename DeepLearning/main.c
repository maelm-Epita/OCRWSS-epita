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
#define LAYER_NUMBER 2
#define LAYER_SIZES {32, 26}
#define DATA_NB 372038
#define MINIBATCH_SIZE 100
#define EPOCHS 50
#define RATE 1e-2
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
  float* input = image_to_input(image_path);
  float* res = feedforward(net, input, NULL, NULL);
  print_float_arr(res, 26);
  printf("Network's prediction : %c\n", output_to_prediction(res));
}

void test_dataset(struct training_set set){
  long i;
  while (1){
    i = rand() % set.data_number;
    printf("%lu - Expected guess : %c\n", i, output_to_prediction((set.data+i)->expected_output));
    print_float_arr((set.data+i)->expected_output, 26);
    input_to_image((set.data+i)->inputs);
  }
}

void test_image(char* path){
  float *i = image_to_input(path);
  input_to_image(i);
}

int main(int argc, char* argv[]) {
  float p[9] = {0,0,0,0,1,1,0,0,0};
  float y[9] = {0,0,1,0,0,0,0,0,0};
  test_cost(p, y);
  float p1[9] = {0,0,0.1,0,0,0,0,0,0};
  float y1[9] = {0,0,1,0,0,0,0,0,0};
  test_cost(p1, y1);
  float p2[9] = {0,0,0.9,0,0,0,0,0,0};
  float y2[9] = {0,0,1,0,0,0,0,0,0};
  test_cost(p2, y2);
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
  }

  // creating training set
  printf("------------\n");
  printf("Training set :\n");
  printf("------------\n");
  float **inputs;
  float **outputs;
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
}
