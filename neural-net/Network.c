#include <stddef.h>
#include <stdlib.h>
#include "math_helpers.h"
#include "Network.h"

float output(float* inputs, float* weights, float bias, size_t inputsize){
  float sum_weighted_inputs = 0;
  for (size_t i=0; i<inputsize; i++){
    sum_weighted_inputs += *(inputs+i)+*(weights+i);
  }
  return sigmoid(sum_weighted_inputs+(bias));
}

float calculate_output(struct Neuron neuron, float* inputs){
  return output(inputs, neuron.weights, neuron.bias, neuron.inputsize);
}

float* feedforward(struct Network net, float* input){
  float* prev_out = input;
  // foreach layer
  for (size_t i=0; i<(net.layernb);i++){
    size_t layer_size = *(net.layersizes+i);
    // create the output array
    float* out = calloc(layer_size, sizeof(float));
    // foreach neuron in the layer
    for (size_t j=0; j<layer_size; j++){
      // add its output to the output array
      *(out+j)=calculate_output(*((*(net.layers+i)).neurons+j), prev_out);
    }
    // the output becomes the previous output thus the input of the next layer
    prev_out = out;
  }
  // the last layer's output is the network's output
  return prev_out;
}

void fill_network(struct Network* network){
  size_t MIN_RAND = -10;
  size_t MAX_RAND = 10;
  // create the layer array
  network->layers = calloc(network->layernb, sizeof(struct Layer));
  size_t layer_inputsize = network->inputsize;
  // foreach layer
  for (size_t i=0; i<network->layernb; i++)
  {
    struct Layer *layer = network->layers+i;
    size_t layersize = *(network->layersizes+i);
    // create the neurons array
    layer->neurons = calloc(layersize, sizeof(struct Neuron));
    // foreach neuron
    for (size_t j=0; j<layersize; j++){
      struct Neuron *neuron = layer->neurons+j;
      // each neuron has input size equal to the amount of neurons in the previous layer
      neuron->inputsize = layer_inputsize;
      // fill bias and weights with random values between MIN_RAND and MAX_RAND
      neuron->bias = float_rand(MIN_RAND, MAX_RAND);
      neuron->weights = rand_float_array(MIN_RAND, MAX_RAND, layer_inputsize);
    }
    layer_inputsize = layersize;
  }
}
