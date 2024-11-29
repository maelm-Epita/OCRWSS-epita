#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "training_data.h"
#include "../shared/arr_helpers.h"

// create the training set from the input and output arrays
struct training_set create_training_set(float** inputs, float** outputs, size_t data_number, size_t input_number){
  // create dataset array
  struct training_data* data = calloc(data_number, sizeof(struct training_data));
  for (size_t i =0; i<data_number; i++){
    struct training_data d = {*(inputs+i), *(outputs+i)};
    *(data+i) = d;
  }
  // create the final training set
  struct training_set set = {input_number, data_number, data};
  return set;
}
// free the set in memory
void free_training_set(struct training_set set){
  for (size_t i=0; i<set.data_number; i++){
    struct training_data data = *(set.data+i);
    free(data.inputs);
    free(data.expected_output);
  }
  free(set.data);
}
void print_training_set(struct training_set set){
  printf("Training set :\n");
  for (size_t i =0; i<set.data_number; i++){
    struct training_data d = *(set.data+i);
    printf("Data %lu : ", i);
    printf("Input : {");
    for (size_t j=0; j<set.input_number; j++){
      printf("%f, ", *(d.inputs+j));
    }
    printf("} ; \n");
    printf("Expected Output : {");
    for (size_t j=0; j<26; j++){
      printf("%f, ", *(d.expected_output+j));
    }
    printf("}\n\n");
  }
}

void shuffle_training_set(struct training_set set)
{
  struct training_data *array = set.data;
  size_t size = set.data_number;
  for (size_t i=0; i<size; i++){
    size_t j = i + rand() / (RAND_MAX / (size - i) + 1);
    struct training_data t = array[j];
    array[j] = array[i];
    array[i] = t;
  }
}

struct minibatch_set create_minibatch_set(struct training_set set, size_t minibatch_size){
  size_t minibatch_nb = set.data_number/minibatch_size;
  struct minibatch_set bset = {minibatch_size, minibatch_nb, NULL};
  bset.mini_batches = calloc(minibatch_nb, sizeof(struct training_set));
  shuffle_training_set(set);
  // todo : fill the minibatches
  for (size_t i=0; i<minibatch_nb; i++){
    struct training_set tset = {set.input_number, minibatch_size, NULL};
    tset.data = calloc(minibatch_size, sizeof(struct training_data));
    for (size_t j=0; j<minibatch_size; j++){
      *(tset.data+j) = *(set.data+(i*minibatch_size)+j);
    }
    *(bset.mini_batches+i) = tset;
  }
  return bset;
}

void free_minibatch_set(struct minibatch_set set){
  for (size_t i=0; i<set.minibatch_number; i++){
    free((set.mini_batches+i)->data);
  }
  free(set.mini_batches);
}
