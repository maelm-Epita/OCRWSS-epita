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

float calculate_output(struct Neuron Neuron, float* inputs); // done
void fill_network(struct Network *Network); // done
void save_network();
struct Network load_network();
float* feedfoward(struct Network *net, float* input); // done

// to create a new network
// we can declare it this way
// Network = {28*28, 3, [26,26,26]}
// this will create a network with an input size (input layer neurons) of 28*28 (784), 3 layers, of size 26 each (26 neurons each)
// note that the layer count and sizes do not count the input and output layer (they do not exist)
// we can then fill the network with layers of neurons of random weights and biases by using fill_network()
// otherwise, we can load an existing network with load_network()
