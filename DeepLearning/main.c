#include "neural-net/Network.h"
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
#define LAYER_NUMBER 4
#define LAYER_SIZES {32, 64, 32, 26}
#define DATA_NB 372038
#define MINIBATCH_SIZE 50
#define EPOCHS 10
#define RATE 5
#define BACKPROP_NUMBER -1
#define DEFAULT_SAVE_PATH "./models/letter.model"
// fork specific
#define NETWORK_NUMBER 8
// thread specific
#define THREAD_NUMBER 8

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
  struct Network net = load_network("./models/letter_model.model");
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

int main(int argc, char* argv[]) {
  // handling arguments
  char* opt = NULL;
  char* existing_path = NULL;
  char* save_opt = NULL;
  char* save_path = NULL;
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
    else{
      errx(EXIT_FAILURE, "Unknown argument \"%s\"\n", opt);
    }
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
}
