#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include "training_data.h"
#include "../neural-net/Network.h"
#include "../shared/arr_helpers.h"

static const float EPS = 1e-3;

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
float* get_Costs(struct Network net, struct training_set minibatch){
  float* costs = calloc(minibatch.data_number, sizeof(float));
  for (size_t i=0; i<minibatch.data_number; i++){
    *(costs+i) = Cost(net, *(minibatch.data+i));
  }
  return costs;
}
float av_Cost(struct Network net, struct training_set minibatch){
  float* costs = get_Costs(net, minibatch);
  float av_cost = av_arr(costs, minibatch.data_number);
  free(costs);
  return av_cost;
}

float av_Cost_PDW(struct Network net, struct Neuron* neuron, size_t windex, struct training_set minibatch){
  float av_cost = av_Cost(net, minibatch);
  float w = *(neuron->weights+windex);
  *(neuron->weights+windex) = w+EPS;
  float d_av_cost = av_Cost(net, minibatch);
  *(neuron->weights+windex) = w;
  return (d_av_cost-av_cost)/EPS;
}
float av_Cost_PDB(struct Network net, struct Neuron* neuron, struct training_set minibatch){
  float av_cost = av_Cost(net, minibatch);
  float b = neuron->bias;
  neuron->bias = b+EPS;
  float d_av_cost = av_Cost(net, minibatch);
  neuron->bias = b;
  return (d_av_cost-av_cost)/EPS;
}

float back_propagate(struct Network* net, struct training_set minibatch, double rate){
  //printf("test modified 1 %f\n", *(net->layers->neurons->weights));
  float av_cost = av_Cost(*net, minibatch);
  for (size_t l=0; l<net->layernb; l++){
    struct Layer* clayer = net->layers+l;
    for (size_t n=0; n<*(net->layersizes+l); n++){
      struct Neuron* cneuron = clayer->neurons+n;
      //float* new_weights = calloc(cneuron->inputsize, sizeof(float));
      for (size_t w=0; w<cneuron->inputsize; w++){
        float acpd = av_Cost_PDW(*net, cneuron, w, minibatch);
        //*(new_weights+w) = *(cneuron->weights+w) - rate*acpd;
        *(cneuron->weights+w) = *(cneuron->weights+w) - rate*acpd;
      }
      /*for (size_t w=0; w<cneuron->inputsize; w++){
        *(cneuron->weights+w) = *(new_weights+w);
      }*/
      float acpd = av_Cost_PDB(*net, cneuron, minibatch);
      cneuron->bias = cneuron->bias - rate*acpd;
      //free(new_weights);
    }
  }
  //printf("test modified 2 %f\n", *(net->layers->neurons->weights));
  return av_cost;
}

float train(struct Network* net, struct training_set set, double rate, size_t minibatch_size, size_t epochs){
  printf("----------------------\n");
  printf("TRAINING NEURAL NETWORK\n");
  printf("----------------------\n");
  for (size_t i=0; i<epochs; i++){
    float av_cost = 0;
    printf("EPOCH %lu\n",i);
    printf("Training.....\n");
    struct minibatch_set mini_set = create_minibatch_set(set, minibatch_size);
    for (size_t j=0; j<mini_set.minibatch_number;j++){
      struct training_set curr_minibatch = *(mini_set.mini_batches+j);
      printf("Mini-batch %lu - Back propagation...\n", j);
      float cost = back_propagate(net, curr_minibatch, rate);
      av_cost += cost;
      printf("Mini batch cost : %f\n", cost);
      printf("Saving network...\n");
      save_network("letter_model", *net);
    }
    av_cost /= mini_set.minibatch_number;
    printf("EPOCH %lu finished, Average cost was : %f\n", i, av_cost);
    free_minibatch_set(mini_set);
    //usleep(100000);
  }
  printf("END OF TRAINING...\n");
  printf("---------------------\n");
  float final_av_cost = av_Cost(*net, set);
  printf("Final average cost of the network on all training data : %f\n", final_av_cost);
  return final_av_cost;
}
