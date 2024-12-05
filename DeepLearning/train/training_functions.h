#pragma once
#include "training_data.h"
#include "../neural-net/Network.h"
#include <stddef.h>

double true_derivative_weight(struct Network net, struct Neuron *neuron, size_t windex, struct training_data data);

double Cost(struct Network net, struct training_data data);
double test_cost(double *p, double *y);
double* get_Costs(struct Network net, struct training_set minibatch);
// if thread_nb <= 1, will not use threading
double av_Cost(struct Network net, struct training_set minibatch, size_t thread_nb);
double Cost_Partialderivative_weight(struct Network net, struct Neuron* neuron, size_t windex, struct training_data data, double cost);
double Cost_Partialderivative_bias(struct Network net, struct Neuron* neuron, struct training_data data, double cost);
double av_Cost_PDW(struct Network net, struct Neuron *neuron, size_t windex, struct training_set minibatch, size_t thread_nb);
double av_Cost_PDB(struct Network net, struct Neuron *neuron,struct training_set minibatch, size_t thread_nb);
// calculate the average of pd of cost when each w or b changes, and apply wk = wk-nAVPDC(w)  or b = b-nAVPDC(b)
void back_propagate(struct Network *net, struct training_data data, double* d_grad_w, double* d_grad_b);
// backprop_nb < 0; do enough backpropagations to go through the entire minibatch for each epoch
// backprop_nb >= 0; do the asked number of backpropagations per epoch
// print_b > 0; print messages
// print_b = 0; don't print messages
// if thread_nb is <= 1, no threading will occur, otherwise calculating costs over minibatch will be distributed among threads
double train(struct Network* net, struct training_set set, double rate, size_t minibatch_size, size_t epochs, 
            char* model_name, int backprop_nb, size_t thread_nb, char print_b);
double train_fork(struct Network base_net, struct training_set set, double rate, size_t minibatch_size, 
                 size_t epochs, size_t backprop_nb, size_t nb_children, size_t thread_nb, char* model_name);
