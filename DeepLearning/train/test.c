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
  const size_t data_nb = 372038;
  const size_t input_size = 28*28;
  FILE *fptr;
  fptr = fopen("./training-set/uncompressed/handwritten_data_785.csv", "r");
  if (fptr == NULL){
    printf("File could not be opened");
    exit(EXIT_FAILURE);
  }
  printf("Loading training data...\n");
  load_training_data(fptr, &inputs, &outputs, data_nb, input_size);
  fclose(fptr);
  printf("Creating training set...\n");
  struct training_set letter_training_set = create_training_set(inputs, outputs, data_nb, input_size);
  free(inputs);
  free(outputs);
  printf("------------\n");
  print_training_set(letter_training_set);
  printf("------------\n");
  free_training_set(letter_training_set);
  return 0;
}
