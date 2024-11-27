#pragma once
#include <stddef.h>

struct training_data{
  float* inputs;
  float* expected_output;
};

struct training_set{
  size_t input_number;
  size_t data_number;
  struct training_data* data;
};

struct minibatch_set{
  size_t minibatch_size;
  size_t minibatch_number;
  struct training_set* mini_batches;
};

// creates training set based on inputs and outputs for each training data 
struct training_set create_training_set(float** inputs, float** outputs, size_t data_number, size_t input_number);
void free_training_set(struct training_set set);
void print_training_set(struct training_set set);
void shuffle_training_set(struct training_set set);
// creates the maximum number of minibatches of minibatch_size size from the training set
struct minibatch_set create_minibatch_set(struct training_set, size_t minibatch_size);
// frees only the array mini_batches and the training data array of each mini_batch
// the rest will be freed by calling free_training_set on the main set which will free all training data
void free_minibatch_set(struct minibatch_set set);
