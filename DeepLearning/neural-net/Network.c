#include "Network.h"
#include "../shared/arr_helpers.h"
#include "../shared/math_helpers.h"
#include <err.h>
#include <locale.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// arbitrary size to calloc with, we assume that no single double/size_t data
// will be more than 20 chars
#define CALLOC_MAX_DATA_SIZE 20

double get_z(double *inputs, double *weights, double bias, size_t inputsize) {
  double sum_weighted_inputs = 0;
  for (size_t i = 0; i < inputsize; i++) {
    sum_weighted_inputs += inputs[i] * weights[i];
  }
  return sum_weighted_inputs + bias;
}

double activation(double z) { return leaky_ReLu(z); }

double calculate_output(struct Neuron neuron, double *inputs) {
  return activation(
      get_z(inputs, neuron.weights, neuron.bias, neuron.inputsize));
}

void calc_weight_bias_amount(struct Network net, size_t *weight_total,
                             size_t *bias_total) {
  size_t w_t = 0;
  size_t b_t = 0;
  for (size_t l = 0; l < net.layernb; l++) {
    // There are inputsize weights for each neuron so inputsize*layersize
    w_t += ((net.layers + l)->neurons)->inputsize * *(net.layersizes + l);
    // There is one bias for each neuron in the layer
    b_t += *(net.layersizes + l);
  }
  *weight_total = w_t;
  *bias_total = b_t;
}

double *feedforward(struct Network net, double *input, double **z_mat,
                    double **a_mat) {
  double *prev_out = input;
  // if we provided z mat and a mat pointers, we are doing the forward pass in
  // the backprop
  if (z_mat != NULL && a_mat != NULL) {
    // size_t dead_neurons = 0;
    for (size_t l = 0; l < net.layernb; l++) {
      size_t layer_size = *(net.layersizes + l);
      double *zs = calloc(layer_size, sizeof(double));
      double *activations = calloc(layer_size, sizeof(double));
      for (size_t n = 0; n < layer_size; n++) {
        struct Neuron neuron = *((net.layers + l)->neurons + n);
        double z =
            get_z(prev_out, neuron.weights, neuron.bias, neuron.inputsize);
        *(zs + n) = z;
        // IF WE ARE ON THE OUTPUT LAYER ; SOFTMAX WILL BE APPLIED (Because we
        // need a probability distribution) WE APPLY SOFTMAX AFTER ALL LOGITS
        // ARE CALCULATED ELSE FOR THE HIDDEN LAYER ; RELU WILL BE APPLIED
        // (Because we need to prevent vanihsing gradient
        if (l < net.layernb - 1) {
          *(activations + n) = activation(z);
        }
      }
      // THIS IS WHERE SOFTMAX IS APPLIED ONTO
      if (l == net.layernb - 1) {
        softmax(zs, layer_size, activations);
      }
      prev_out = activations;
      *(z_mat + l) = zs;
      *(a_mat + l) = activations;
    }
  }
  return *(a_mat + net.layernb - 1);
}

char *neuron_to_str(struct Neuron neur) {
  char *wstr = double_arr_to_str(neur.weights, neur.inputsize);
  char *str = NULL;
  size_t b = asprintf(&str, "%s %f", wstr, neur.bias);
  if (b < 1) {
    printf("asprintf failed");
    exit(EXIT_FAILURE);
  }
  free(wstr);
  return str;
}

void free_network(struct Network *net) {
  for (size_t i = 0; i < net->layernb; i++) {
    struct Layer *layer = (net->layers + i);
    for (size_t j = 0; j < net->layersizes[i]; j++) {
      struct Neuron *neuron = (layer->neurons + j);
      free(neuron->weights);
    }
    free(layer->neurons);
  }
  free(net->layers);
}
void free_network_loaded(struct Network *net) {
  free_network(net);
  free(net->layersizes);
}

void fill_network(struct Network *network) {
  srand(time(0));
  // create the layer array
  network->layers = calloc(network->layernb, sizeof(struct Layer));
  size_t layer_inputsize = network->inputsize;
  // foreach layer
  for (size_t i = 0; i < network->layernb; i++) {
    struct Layer *layer = network->layers + i;
    size_t layersize = *(network->layersizes + i);
    // create the neurons array
    layer->neurons = calloc(layersize, sizeof(struct Neuron));
    // foreach neuron
    for (size_t j = 0; j < layersize; j++) {
      struct Neuron *neuron = layer->neurons + j;
      // each neuron has input size equal to the amount of neurons in the
      // previous layer
      neuron->inputsize = layer_inputsize;
      // fill bias and weights with random values between MIN_RAND and MAX_RAND
      neuron->bias = 0;
      neuron->weights = he_weight_array(layer_inputsize);
    }
    layer_inputsize = layersize;
  }
}

void save_network(char *name, struct Network net) {
  // create file / open file
  char *modelname = NULL;
  size_t b = asprintf(&modelname, "./%s", name);
  if (b < 1) {
    printf("asprintf failed");
    exit(EXIT_FAILURE);
  }
  FILE *fptr = fopen(modelname, "w");
  free(modelname);
  // start writing data
  // first line : $inputsize $layernb
  fprintf(fptr, "%lu %lu\n", net.inputsize, net.layernb);
  // second line : $layersizes separated by spaces
  char *lsizes = sze_arr_to_str(net.layersizes, net.layernb);
  fprintf(fptr, "%s\n", lsizes);
  free(lsizes);
  // rest of the lines : 1 line = 1 neuron :
  for (size_t i = 0; i < net.layernb; i++) {
    struct Layer layer = *(net.layers + i);
    for (size_t j = 0; j < *(net.layersizes + i); j++) {
      struct Neuron neuron = *(layer.neurons + j);
      char *neu_str = neuron_to_str(neuron);
      fprintf(fptr, "%s\n", neu_str);
      free(neu_str);
    }
  }
  fclose(fptr);
}

struct Network load_network(char *path) {
  struct Network network;
  // open file
  FILE *fptr = fopen(path, "r");
  // buffer to read char by char into
  int buf = 0;
  // current line number
  size_t line = 0;
  // current index of the data on the line (basically the column if treating the
  // file as a 2d array)
  size_t line_column = 0;
  // current index in the current data buffer
  size_t cdi = 0;
  // current index of the layer we are "on"
  size_t layindex = 0;
  // current index of the neuron we are "on"
  size_t neurindex = 0;
  char *curr_data = calloc(CALLOC_MAX_DATA_SIZE, sizeof(char));
  while (buf != EOF) {
    buf = fgetc(fptr);
    // when we find a separator (space, newline) we declare the current data as
    // finished and we process it accordingly the last line will always end with
    // a newline, therefore we dont consider EOF as a separator, we just dont
    // care for it
    if (buf == ' ' || buf == '\n') {
      printf("found sep: %c\n", buf);
      *(curr_data + cdi) = 0;
      cdi = 0;
      // inputsize and layernumber line
      if (line == 0) {
        // convert our buffer string to a size
        size_t s_data = atoi(curr_data);
        if (s_data == 0) {
          printf("data could not be converted to an int or was null");
          exit(EXIT_FAILURE);
        }
        // first data in the first line is the inputsize, second is the number
        // of layers
        if (line_column == 0) {
          network.inputsize = s_data;
        } else {
          network.layernb = s_data;
          // we can now declare the layer array and layersizes array since we
          // know their size
          network.layers = calloc(s_data, sizeof(struct Layer));
          network.layersizes = calloc(s_data, sizeof(struct Layer));
        }
      }
      // layersizes line
      else if (line == 1) {
        // convert buffer str to size
        size_t s_data = atoi(curr_data);
        if (s_data == 0)
          errx(EXIT_FAILURE,
               "data could not be converted to an int or was null");

        // set appropriate element of layersizes to the data
        network.layersizes[line_column] = s_data;
        // we can then calloc that layer's neuron array
        (network.layers + line_column)->neurons =
            calloc(s_data, sizeof(struct Neuron));
      }
      // rest of the lines contain info for one neuron each ("$weight1 $weight2
      // ... $weightn_$bias)
      else {
        //
        struct Layer layer = network.layers[layindex];
        struct Neuron *neuron = layer.neurons + neurindex;
        // if the neuron isnt initialized
        if (neuron->inputsize == 0) {
          // initialize, the size is either the input size of the network (if
          // first layer), or the number of neurons in the prev layer
          if (layindex == 0) {
            neuron->inputsize = network.inputsize;
          } else {
            neuron->inputsize = network.layersizes[layindex - 1];
          }
          neuron->weights = calloc(neuron->inputsize, sizeof(double));
        }
        // convert data buffer to double
        // Sauvegarder la locale actuelle
        char *old_locale = setlocale(LC_NUMERIC, NULL);
        old_locale = old_locale ? strdup(old_locale) : NULL;

        // Fixer la locale à "C" (format américain avec le point)
        setlocale(LC_NUMERIC, "C");
        char *endptr = "";
        double f_data = strtod(curr_data, &endptr);
        if (*endptr != '\0') {
          errx(EXIT_FAILURE,
               "data could not be converted to a double or was null, %s, ended "
               "on : %c, double data : %f\n",
               curr_data, *endptr, f_data);
        }
        // if the sep was a newline or a EOF, we know we were on the last column
        // of the line, thus the data is the bias we can then increase the
        // neuron index and then layer index if need be
        if (buf == EOF || buf == '\n') {
          // we set the bias
          neuron->bias = f_data;
          neurindex++;
          // if we were on the last neuron of the layer
          if (neurindex == network.layersizes[layindex]) {
            layindex++;
            neurindex = 0;
            // printf("%c\n", buf);
          }
        }
        // otherwise the data is a weight
        else
          neuron->weights[line_column] = f_data;
      }
      // we either increment the column or reset it and increment the line
      if (buf == ' ')
        line_column++;
      else {
        line_column = 0;
        line++;
      }
      // at the very end we reset curr_data
      str_arr_clear(curr_data, CALLOC_MAX_DATA_SIZE);
    }
    // if we did not find a separator we just keep filling the current data
    // buffer
    else {
      curr_data[cdi] = buf;
      cdi++;
    }
  }
  free(curr_data);
  fclose(fptr);
  return network;
}
