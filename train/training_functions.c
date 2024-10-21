#include <stdlib.h>
#include <math.h>
#include "training_data.h"
#include "../neural-net/Network.h"
#include "../shared/arr_helpers.h"

static const float EPS = 1e-8;

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
float av_Cost(struct Network net, struct training_set minibatch){
  float* costs = calloc(minibatch.data_number, sizeof(float));
  for (size_t i=0; i<minibatch.data_number; i++){
    *(costs+i) = Cost(net, *(minibatch.data+i));
  }
  return av_arr(costs, minibatch.data_number);
}

float Cost_Partialderivative_weight(struct Network net, struct Neuron* neuron, size_t windex, struct training_data data){
  float w = *(neuron->weights+windex);
  float reg_cost = Cost(net, data);
  *(neuron->weights+windex) = w+EPS;
  float d_cost = Cost(net, data);
  *(neuron->weights+windex) = w;
  return (d_cost-reg_cost)/EPS;
}
float Cost_Partialderivative_bias(struct Network net, struct Neuron* neuron, struct training_data data){
  float b = neuron->bias;
  float reg_cost = Cost(net, data);
  neuron->bias = b+EPS;
  float d_cost = Cost(net, data);
  neuron->bias = b;
  return (d_cost-reg_cost)/EPS;
}
float av_CPDW(struct Network net, struct Neuron* neuron, size_t windex, struct training_set minibatch){
  float* costs = calloc(minibatch.data_number, sizeof(float));
  for (size_t i=0; i<minibatch.data_number; i++){
    *(costs+i) = Cost_Partialderivative_weight(net, neuron, windex, *(minibatch.data+i));
  }
  float av = av_arr(costs, minibatch.data_number);
  free(costs);
  return av;
}
float av_CPDB(struct Network net, struct Neuron* neuron, struct training_set minibatch){
  float* costs = calloc(minibatch.data_number, sizeof(float));
  for (size_t i=0; i<minibatch.data_number; i++){
    *(costs+i) = Cost_Partialderivative_bias(net, neuron, *(minibatch.data+i));
  }
  float av = av_arr(costs, minibatch.data_number);
  free(costs);
  return av;
}

void back_propagate(struct Network* net, struct training_set minibatch, float rate){
  for (size_t l=0; l<net->layernb; l++){
    struct Layer* clayer = net->layers+l;
    for (size_t n=0; n<*(net->layersizes+l); n++){
      struct Neuron* cneuron = clayer->neurons+n;
      for (size_t w=0; w<cneuron->inputsize; w++){
        float acpd = av_CPDW(*net, cneuron, w, minibatch);
        *(cneuron->weights+w) = *(cneuron->weights+w) - rate*acpd;
      }
      float acpd = av_CPDB(*net, cneuron, minibatch);
      cneuron->bias = cneuron->bias - rate*acpd;
    }
  }
}
