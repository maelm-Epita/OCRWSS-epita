#include <stdlib.h>
#include <stdio.h>

#include "neural-net/Network.h"
#include "shared/arr_helpers.h"
#include "train/training_data.h"
#include "train/training_data_loader.h"
#include "train/training_functions.h"

void letter_train(){
  // defining constants
  const size_t DATA_NB = 372038;
  const size_t INPUT_SIZE = 28*28;
  const size_t MINIBATCH_SIZE = 800;
  const size_t EPOCHS = 3;
  const double RATE = 1e-3;
  // creating training set
  printf("------------\n");
  printf("Training set :\n");
  printf("------------\n");
  float **inputs;
  float **outputs;
  FILE *fptr;
  fptr = fopen("./train/training-set/uncompressed/handwritten_data_785.csv", "r");
  if (fptr == NULL){
    printf("File could not be opened");
    exit(EXIT_FAILURE);
  }
  printf("Loading training data...\n");
  load_training_data(fptr, &inputs, &outputs, DATA_NB, INPUT_SIZE);
  fclose(fptr);
  printf("Creating training set...\n");
  struct training_set letter_training_set = create_training_set(inputs, outputs, DATA_NB, INPUT_SIZE);
  free(inputs);
  free(outputs);
  // creating network and filling it
  printf("------------\n");
  printf("Neural network :\n");
  printf("------------\n");
  size_t layersizes[3] = {200,56,26};
  struct Network net = {28*28, 3, layersizes};
  printf("Creating and filling network with random initial weights and biases\n");
  fill_network(&net);
  // training the network
  printf("--------------------\n");
  printf("Training the network\n");
  train(&net, letter_training_set, RATE, MINIBATCH_SIZE, EPOCHS);
  printf("Finished training the network\n");
  printf("--------------------\n");
  // saving it
  printf("Saving the model\n");
  save_network("testmodel", net);
  // cleanup
  printf("Freeing the heap\n");
  free_network(&net);
  free_training_set(letter_training_set);
  printf("------------\n");
  printf("Finished\n");
}

void xor_train(){
  const size_t DATA_NB = 4;
  const size_t INPUT_SIZE = 2;
  const size_t MINIBATCH_SIZE = 2;
  const size_t EPOCHS = 1000*1000;
  const double RATE = 1e-3;
  float input1[2] = {0,0};
  float output1[1] = {0};
  float input2[2] = {0,1};
  float output2[1] = {1};
  float input3[2] = {1,0};
  float output3[1] = {1};
  float input4[2] = {1,1};
  float output4[1] = {0};
  float **inputs = calloc(DATA_NB, sizeof(float*));
  *(inputs+0) = input1;
  *(inputs+1) = input2;
  *(inputs+2) = input3;
  *(inputs+3) = input4;
  float **outputs = calloc(DATA_NB, sizeof(float*));
  *(outputs+0) = output1;
  *(outputs+1) = output2;
  *(outputs+2) = output3;
  *(outputs+3) = output4;
  struct training_set xor_set = create_training_set(inputs, outputs, DATA_NB, INPUT_SIZE);
  free(inputs);
  free(outputs);
  //
  size_t layersizes[2] = {2,1};
  struct Network net = {2, 2, layersizes};
  fill_network(&net);
  //
  train(&net, xor_set, RATE, MINIBATCH_SIZE, EPOCHS);
  //
  free_network(&net);
  free_training_set(xor_set);
}

int main(){
  xor_train();
}
