#pragma once
#include <stddef.h>

struct Neuron{
  size_t inputsize;
  float* weights;
  float bias;
};

struct Layer{
  struct Neuron* neurons;
};

struct Network{
  size_t inputsize;
  size_t layernb;
  size_t* layersizes;
  struct Layer* layers;
};

// calculates output of a neuron for the given input
float calculate_output(struct Neuron Neuron, float* inputs);
// sets all network variables and fills weights and biases with random initial values
void fill_network(struct Network *Network); 
// saves a neural network in a .model file with the following structure
// first line contains 2 columns, first is the inputsize of the network, second is the layer number n
// second line contains n columns, each column is the size of the respective layer
// all subsequent lines until EOF represent a single neuron;
// they each have N+1 columns where N is the number of inputs of the neuron (which can be determined from the previous layersize or inputsize of network)
// the first N columns are the weights, the last column is the bias
void save_network(char *modelname, struct Network net);
// loads network from a file formatted in the same way as the save function
struct Network load_network(char *path);
// calculates the output of the network based on the input
float* feedforward(struct Network net, float* input);
// free a network, should be called when a network which was hand created and filled is no longer needed
// if you created the layersizes array on the stack
void free_network(struct Network *net); 
// free a network, should be called when a network which was loaded from a file is no longer needed
// calls free_network and also frees network->layersizes which is callocd when loading
void free_network_loaded(struct Network *net);
char* neuron_to_str(struct Neuron neuron);

// to create a new network
// we can declare it this way
// Network = {28*28, 3, [26,26,26]}
// this will create a network with an input size (input layer neurons) of 28*28 (784), 3 layers, of size 26 each (26 neurons each)
// note that the layer count and sizes do not count the input and output layer (they do not exist)
// we can then fill the network with layers of neurons of random weights and biases by using fill_network()
// otherwise, we can load an existing network with load_network()
