#include <stdlib.h>
#include <stdio.h>

#include "neural-net/Network.h"
#include "neural-net/arr_helpers.h"
#include "train/training_data.h"
#include "train/training_data_loader.h"

int main(){
  printf("training set\n");
  printf("------------\n");
  float **inputs;
  float **outputs;
  const size_t data_nb = 372038;
  const size_t input_size = 28*28;
  FILE *fptr;
  fptr = fopen("./train/training-set/uncompressed/handwritten_data_785.csv", "r");
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
  printf("neural nets\n");
  printf("------------\n");
  printf("creating first model\n");
  size_t layersizes[3] = {200,56,26};
  struct Network net = {28*28, 3, layersizes};
  printf("filling\n");
  fill_network(&net);
  printf("feed-forward\n");
  // feedforward with the first training data of the set
  float* out = feedforward(net, letter_training_set.data->inputs);
  printf("res :\n");
  print_float_arr(out, 26);
  printf("saving\n");
  save_network("testmodel", net);
  printf("freeing\n");
  free_network(&net);
  free_training_set(letter_training_set);
  printf("------------\n");
}
