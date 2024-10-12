#include <stdio.h>
#include <stdlib.h>
#include "training_data_loader.h"
#include "training_data.h"
// helper
void free_multi_arr(float** arr, size_t size){
  for (size_t i=0; i<size; i++){
    free(*(arr+i));
  }
  free(arr);
}

int main(){
  float **inputs;
  float **outputs;
  size_t data_nb;
  size_t input_size;
  FILE *fptr;
  fptr = fopen("./test-set/data.d", "r");
  if (fptr == NULL){
    printf("File could not be opened");
    exit(EXIT_FAILURE);
  }
  load_training_data(fptr, &inputs, &outputs, &data_nb, &input_size);
  fclose(fptr);
  struct training_set letter_training_set = create_training_set(inputs, outputs, data_nb, input_size);
  free(inputs);
  free(outputs);
  print_training_set(letter_training_set);
  free_training_set(letter_training_set);
  return 0;
}
