#pragma once
#include <stddef.h>

float Cost(struct Network net, struct training_data data);
float* get_Costs(struct Network net, struct training_set minibatch);
float av_Cost(struct Network net, struct training_set minibatch);
float Cost_Partialderivative_weight(struct Network net, struct Neuron* neuron, size_t windex, struct training_data data, float cost);
float Cost_Partialderivative_bias(struct Network net, struct Neuron* neuron, struct training_data data, float cost);
float av_CPDW(struct Network net, struct Neuron* neuron, size_t windex, struct training_set minibatch, float* costs);
float av_CPDB(struct Network net, struct Neuron* neuron, struct training_set minibatch, float* costs);
// calculate the average of pd of cost when each w or b changes, and apply wk = wk-nAVPDC(w)  or b = b-nAVPDC(b)
float back_propagate(struct Network* net, struct training_set minibatch, float rate);
// backprop_nb < 0; do enough backpropagations to go through the entire minibatch for each epoch
// backprop_nb >= 0; do the asked number of backpropagations per epoch
// print_b > 0; print messages
// print_b = 0; don't print messages
float train(struct Network* net, struct training_set set, double rate, size_t minibatch_size, size_t epochs, char* model_name, size_t backprop_nb, char print_b);
float _train_fork(struct Network* net, struct training_set curr_minibatch, double rate);
float train_fork(struct Network base_net, struct training_set set, double rate, size_t minibatch_size, 
                 size_t epochs, size_t backprop_nb, size_t nb_children, char* model_name);
