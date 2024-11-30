#include "threading.h"
#include "../train/training_functions.h"
#include <stdio.h>
#include <stdlib.h>

void *worker (void *arg){
  struct thread_data *data = (struct thread_data*)arg;
  for (size_t w = data->start; w<data->end; w++){
    float acpd = av_Cost_PDW(*data->net, data->neuron, w, *data->minibatch);
    *(data->neuron->weights + w) = *(data->neuron->weights + w) - data->rate * acpd;
  }
  pthread_exit(NULL);
}

struct thread_data* create_thread_data_array(size_t thread_nb, size_t weights_nb, float rate,
                                             struct training_set *minibatch, struct Network *net,
                                             struct Neuron* neuron){
  struct thread_data* th_data_arr = calloc(thread_nb, sizeof(struct thread_data));
  size_t w_per_thread= weights_nb/thread_nb;
  size_t w_per_thread_remainder = weights_nb/thread_nb+weights_nb%thread_nb;
  for (size_t i = 0; i<thread_nb; i++){
    struct thread_data th_d = *(th_data_arr+i);
    th_d.id = i;
    th_d.start = i*w_per_thread;
    if (i == thread_nb-1){
      th_d.end = th_d.start+w_per_thread_remainder;
    }
    else{
      th_d.end = th_d.start+w_per_thread;
    }
    th_d.rate = rate;
    th_d.minibatch = minibatch;
    th_d.net = net;
    th_d.neuron = neuron;
    *(th_data_arr+i) = th_d;
  }
  return th_data_arr;
}
