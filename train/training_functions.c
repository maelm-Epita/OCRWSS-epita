#include <stdlib.h>
#include <math.h>
#include "training_data.h"
#include "../neural-net/Network.h"
#include "../shared/arr_helpers.h"


float Cost(struct Network net, struct training_data data){
  // the output size is the size of the last layer
  size_t outputsize = *(net.layersizes+net.layernb-1);
  float* output = feedforward(net, data.inputs);
  float* diff = sub_arr(output, data.expected_output, outputsize);
  float cost = pow(norm(diff, outputsize),2)/2;
  free(diff);
  free(output);
  return cost;
}
float av_Cost(float* costs, size_t size){
  return av_arr(costs, size);
}
