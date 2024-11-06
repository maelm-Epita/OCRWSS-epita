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
  const size_t MINIBATCH_SIZE = 50;
  const size_t EPOCHS = 3;
  const double RATE = 15;
  // creating training set
  printf("------------\n");
  printf("Training set :\n");
  printf("------------\n");
  float **inputs;
  float **outputs;
  FILE *fptr;
  fptr = fopen("./training-set/huge_data.csv", "r");
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
  size_t layersizes[2] = {26,26};
  struct Network net = {28*28, 2, layersizes, NULL};
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

void letter_train_existing(){
  // defining constants
  const size_t DATA_NB = 372038;
  const size_t INPUT_SIZE = 28*28;
  const size_t MINIBATCH_SIZE = 50;
  const size_t EPOCHS = 3;
  const double RATE = 15;
  // creating training set
  printf("------------\n");
  printf("Training set :\n");
  printf("------------\n");
  float **inputs;
  float **outputs;
  FILE *fptr;
  fptr = fopen("./training-set/huge_data.csv", "r");
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
  struct Network net = load_network("./letter_model.model");
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
  //const size_t EPOCHS = 10000;
  const double RATE = 15;
  float input1[2] = {0,0};
  float output1[1] = {1};
  float input2[2] = {0,1};
  float output2[1] = {0};
  float input3[2] = {1,0};
  float output3[1] = {0};
  float input4[2] = {1,1};
  float output4[1] = {1};
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
  //
  size_t layersizes[2] = {2,1};
  struct Network net = {2, 2, layersizes, NULL};
  float cost = 1;
  while (cost>1e-4){
    if (cost!=1){
      free_network(&net);
    }
    fill_network(&net);
    cost = train(&net, xor_set, RATE, MINIBATCH_SIZE, 1000);
  }
  cost = train(&net, xor_set, RATE, MINIBATCH_SIZE, 1000);
  // see everything
  save_network("testmodelxor", net);
  // test out
  float* foutput1 = feedforward(net, input1);
  float* foutput2 = feedforward(net, input2);
  float* foutput3 = feedforward(net, input3);
  float* foutput4 = feedforward(net, input4);
  printf("output 1 :\n");
  print_float_arr(input1, 2);
  print_float_arr(output1, 1);
  print_float_arr(foutput1, 1);
  printf("output 2 :\n");
  print_float_arr(input2, 2);
  print_float_arr(output2, 1);
  print_float_arr(foutput2, 1);
  printf("output 3 :\n");
  print_float_arr(input3, 2);
  print_float_arr(output3, 1);
  print_float_arr(foutput3, 1);
  printf("output 4 :\n");
  print_float_arr(input4, 2);
  print_float_arr(output4, 1);
  print_float_arr(foutput4, 1);
  free(foutput1);
  free(foutput2);
  free(foutput3);
  free(foutput4);
  free(inputs);
  free(outputs);
  //
  free(xor_set.data);
  free_network(&net);
}

int main(){
  letter_train();
}
