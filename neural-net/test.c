#include <stdio.h>
#include <stdlib.h>
#include "arr_helpers.h"
#include "Network.h"


int main(){
  printf("creating first model\n");
  size_t layersizes[3] = {26,26,26};
  struct Network net = {28*28, 3, layersizes};
  printf("filling\n");
  fill_network(&net);
  printf("saving\n");
  save_network("testmodel", net);
  printf("freeing\n");
  free_network(&net);
  printf("loading first model\n");
  struct Network copy = load_network("./testmodel.model");
  printf("saving\n");
  save_network("testmodelcp", copy);
  printf("freeing\n");
  free_network_loaded(&copy);
}
