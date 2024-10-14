struct Neuron{
  size_t inputsize;
  float* weights;
  float bias;
};

struct Layer{
  struct Neuron* neurons;
};

struct Network{
  size_t layernb;
  size_t* layersizes;
  struct Layer* layers;
};

float calculate_output(struct Neuron Neuron, float* inputs); // done
void fill_network(struct Network *Network); 
void save_network();
struct Network load_network();
float* feedfoward(struct Network *net, float* input); // done

// to create a new network
// we can declare it this way
// Network = {3, [26,26,26]}
// this will create a network with 3 layers, of size 26 each (26 neurons each)
// note that this does not count the input and output layer (they do not exist)
// we can then load an existing set of layers, neurons, weights+biases by using load_network()
// or we can fill the network with layers of neurons of random weights and biases by using fill_network()
