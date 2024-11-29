#include "neural-net/Network.h"
#include "shared/arr_helpers.h"
#include "train/training_data.h"
#include "train/training_data_loader.h"
#include "train/training_functions.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <err.h>

// defining constants
#define INPUT_SIZE 28 * 28
#define LAYER_NUMBER 4
#define DATA_NB 372038
#define MINIBATCH_SIZE 50
#define EPOCHS 1
#define RATE 5
#define BACKPROP_NUMBER 1
#define MODEL_NAME "./models/letter.model"
// fork specific
#define NETWORK_NUMBER 8
// thread specific
#define THREAD_NUMBER 8

void letter_train() {
  // creating training set
  printf("------------\n");
  printf("Training set :\n");
  printf("------------\n");
  float **inputs;
  float **outputs;
  FILE *fptr;
  fptr = fopen("./datas.csv", "r");
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
  // creating network and filling it
  printf("------------\n");
  printf("Neural network :\n");
  printf("------------\n");
  size_t layersizes[4] = {35, 64, 32, 26};
  struct Network net = {28 * 28, 4, layersizes, NULL};
  printf(
      "Creating and filling network with random initial weights and biases\n");
  fill_network(&net);
  // training the network
  printf("--------------------\n");
  printf("Training the network\n");
  //train(&net, letter_training_set, RATE, MINIBATCH_SIZE, EPOCHS);
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

void _letter_train_fork() {
  const size_t NB_CHILDREN = 8;
  // pipes
  // costfds is an array of pipes, each child has its own pipe
  // they will be used once to write the cost of each child's nn
  // then each will be closed
  // they will be initialized in the loop
  int (*costfds)[2] = calloc(NB_CHILDREN, sizeof(int[2]));
  // resfd is a single pipe through which the parent
  // will write the pid of the pipe which should save
  int resfd[2];
  if (pipe(resfd) < 0) {
    printf("could not pipe res pipe");
    err(EXIT_FAILURE, "pipes failed");
  }
  // var of our nn
  const size_t REPETITIONS = 10;
  size_t layersizes[3] = {32, 64, 26};
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
  struct training_set letter_training_set = create_training_set(inputs, outputs, DATA_NB, INPUT_SIZE);
  free(inputs);
  free(outputs);
  struct minibatch_set mini_set =
    create_minibatch_set(letter_training_set, MINIBATCH_SIZE);

  // create many Networks with fork, act for each
  for (size_t i = 0; i < NB_CHILDREN; i++) {
    // delay for random
    sleep(5);
    // initialize the child's pipe
    if (pipe(costfds[i]) < 0) {
      errx(EXIT_FAILURE, "Failed to pipe child %lu\n", i);
    }
    printf("cost pipe %lu piped\n", i);
    // forking
    int id = fork();
    if (id < 0) {
      errx(EXIT_FAILURE, "Failed to fork\n");
    }
    // child code
    else if (id == 0) {
      printf("in child %lu\n", i);
      int pid = getpid();
      // close unused pipe ends
      close(costfds[i][0]); // read close
      close(resfd[1]); // write close
      // initialize and train
      struct Network net = {28 * 28, 3, layersizes, NULL};
      fill_network(&net);
      for (size_t j = 0; j < REPETITIONS; j++) {
        printf("Child %lu, pid %d, repetition %lu...\n", i, pid, j);
        _train_fork(&net, *(mini_set.mini_batches + j), RATE);
      }
      float cost = Cost(net, *letter_training_set.data);
      printf("trained child %lu, cost is %.6f\n", i, cost);
      // sending results to father thread
      // these two calls should run right after one another, 
      // if another pipe interrupts, it would mess the entire process
      printf("writing cost of child : %f\n", cost);
      if (write(costfds[i][1], &cost, sizeof(cost)) < (long)sizeof(cost)){
        errx(EXIT_FAILURE, "Failed to write all cost\n");
      }
      printf("writing pid of child : %d\n", pid);
      if (write(costfds[i][1], &pid, sizeof(pid)) < (long)sizeof(cost)){
        errx(EXIT_FAILURE, "Failed to write all pid\n");
      }
      printf("wrote\n");
      // we can send eof through the pipe by closing the writing end
      close(costfds[i][1]);
      // now we will read the result send by the father
      // all children will be blocked on the read instruction until
      // the father sends bytes through the pipe
      int selected;
      printf("child %lu attempting to read from parent\n", i);
      if (read(resfd[0], &selected, sizeof(int)) < (long)sizeof(int)){
        errx(EXIT_FAILURE, "Failed to read all pid from parent\n");
      }
      printf("read from parent: %d\n", selected);
      // the child saves or not depending on the instruction from the father
      printf("pid of child %lu is: %d\n", i, pid);
      if (pid == selected) {
        printf("child %d saved\n", pid);
        save_network("models/best_child_model", net);
      }
      printf("exiting\n");
      // cleaning up
      free_network(&net);
      close(resfd[0]);
      // exiting
      exit(EXIT_SUCCESS);
    }
  }
  // father code
  // outside of the loop is out of range for the children, the father will create a
  // child for each iteration, ignore all other instructions and finally come out of the loop
  // close unused ends of res pipe
  close(resfd[0]); // read close
  // the father process should be waiting for execution of all children until they write their results
  // reading from each child's write pipe to know each pid and cost in order
  // to find the minimum cost's child pid
  // we do not need to worry about timing the reads because read will block execution until it receives data
  float min_cost = 1;
  int min_pid;
  float temp_cost;
  int temp_pid;
  for (size_t i = 0; i < NB_CHILDREN; i++) {
    // we will close the unused end of the cost pipe of each child
    printf("closing unused end of pipe for child %lu\n",i);
    close(costfds[i][1]);
    printf("reading from child %lu\n", i);
    if (read(costfds[i][0], &temp_cost, sizeof(float)) < (long)sizeof(float)){
        errx(EXIT_FAILURE, "Failed to read all cost from child %lu\n", i);
    }
    if (read(costfds[i][0], &temp_pid, sizeof(int)) < (long)sizeof(int)){
        errx(EXIT_FAILURE, "Failed to read all pid from child %lu\n", i);
    }
    if (temp_cost < min_cost) {
      min_cost = temp_cost;
      min_pid = temp_pid;
    }
    printf("read from child %lu : %f, %d\n", i, temp_cost, temp_pid);
    // after reading from the child's cost pipe, we can close the reading end
    close(costfds[i][0]);
  }
  printf("correct pid is %d\n", min_pid);
  // the father process will write the correct pid once for each child, so they can all read it
  for (size_t i = 0; i < NB_CHILDREN; i++) {
    printf("writing pid\n");
    if (write(resfd[1], &min_pid, sizeof(int)) < (long)sizeof(int)){
      errx(EXIT_FAILURE, "Failed to read all pid from child %lu\n", i);
    }
  }
  // it can then close the writing end so that the children reach EOF
  close(resfd[1]);
  // then the father will wait until all children have exited
  for (size_t i = 0; i < NB_CHILDREN; i++) {
    printf("waiting for child to exit\n");
    wait(NULL);
  }
  // we can now gain access to the lowest cost network
  struct Network nn = load_network("./models/best_child_model.model");
  printf("the minimal cost is %f of the %d child\n", min_cost, min_pid);
  printf("Finished training the network\n");
  printf("--------------------\n");
  // cleanup
  printf("Freeing the heap\n");
  free_training_set(letter_training_set);
  printf("------------\n");
  printf("Finished\n");
  free_network(&nn);
}

void letter_train_existing() {
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
  // creating network and filling it
  printf("------------\n");
  printf("Neural network :\n");
  printf("------------\n");
  struct Network net = load_network("./models/letter_model.model");
  // training the network
  printf("--------------------\n");
  printf("Training the network\n");
  //train(&net, letter_training_set, RATE, MINIBATCH_SIZE, EPOCHS, BACKPROP_NUMBER, NETWORK_NUMBER, MODEL_NAME);
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
/*
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
  struct training_set xor_set = create_training_set(inputs, outputs, DATA_NB,
INPUT_SIZE);
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
*/

void letter_train_fork(){
  // creating training set
  printf("------------\n");
  printf("Training set :\n");
  printf("------------\n");
  float **inputs;
  float **outputs;
  FILE *fptr;
  fptr = fopen("./training-set/huge_data.csv", "r");
  if (fptr == NULL) {
    printf("File could not be opened\n");
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
  // creating model network and filling it
  printf("------------\n");
  printf("Neural network :\n");
  size_t layersizes[LAYER_NUMBER] = {35, 64, 32, 26};
  struct Network net = {INPUT_SIZE, 4, layersizes, NULL};
  // training the network
  printf("Training the networks\n");
  printf("------------\n");
  train_fork(net, letter_training_set, RATE, MINIBATCH_SIZE, EPOCHS, BACKPROP_NUMBER, NETWORK_NUMBER, MODEL_NAME);
  printf("------------\n");
  printf("Finished training the network\n");
  printf("--------------------\n");
  // saving it
  // cleanup
  printf("-> Freeing the heap\n");
  free_network(&net);
  free_training_set(letter_training_set);
  printf("-> Finished\n");
}

int main() { letter_train_fork(); }
