#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "training_data.h"

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

