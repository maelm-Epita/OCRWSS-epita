#include <stdio.h>
#include <stdlib.h>
#include "arr_helpers.h"
#include "Network.h"


int main(){
  size_t layersizes[3] = {26,26,26};
  struct Network net = {28*28, 3, layersizes};
  fill_network(&net);
  save_network("testmodel", net);
  free_network(&net);
}
