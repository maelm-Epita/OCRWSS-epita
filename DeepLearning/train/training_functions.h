#pragma once

float Cost(struct Network net, struct training_data data);
float* get_Costs(struct Network net, struct training_set minibatch);
float av_Cost(struct Network net, struct training_set minibatch);
float Cost_Partialderivative_weight(struct Network net, struct Neuron* neuron, size_t windex, struct training_data data, float cost);
float Cost_Partialderivative_bias(struct Network net, struct Neuron* neuron, struct training_data data, float cost);
float av_CPDW(struct Network net, struct Neuron* neuron, size_t windex, struct training_set minibatch, float* costs);
float av_CPDB(struct Network net, struct Neuron* neuron, struct training_set minibatch, float* costs);
// calculate the average of pd of cost when each w or b changes, and apply wk = wk-nAVPDC(w)  or b = b-nAVPDC(b)
void back_propagate(struct Network* net, struct training_set minibatch, float rate);
float train(struct Network* net, struct training_set set, double rate, size_t minibatch_size, size_t epochs);

