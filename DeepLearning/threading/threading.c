#include "threading.h"
#include "../train/training_functions.h"
#include <stdio.h>
#include <stdlib.h>

void *worker (void *arg){
  struct thread_data *data = (struct thread_data*)arg;
  float sum = 0;
  struct Network net = *data->net;
  struct training_set* minibatch = data->minibatch;
  for (size_t d = data->start; d<data->end; d++){
    sum += Cost(net, *(minibatch->data+d));
  }
  (data->sum) = sum;
  //printf("thread %lu ; sum: %f\n", data->id, *data->sum);
  pthread_exit(NULL);
}

struct thread_data* create_thread_data_array(size_t thread_nb, struct training_set *minibatch, 
                                             struct Network *net){
  struct thread_data* th_data_arr = calloc(thread_nb, sizeof(struct thread_data));
  size_t data_number = minibatch->data_number;
  size_t d_per_thread = data_number/thread_nb;
  size_t d_per_thread_remainder = data_number/thread_nb+data_number%thread_nb;
  //printf("found : d_per_thread : %lu, d_per_thread_remainder : %lu\n", d_per_thread, d_per_thread_remainder);
  for (size_t i = 0; i<thread_nb; i++){
    struct thread_data th_d = *(th_data_arr+i);
    th_d.id = i;
    th_d.start = i*d_per_thread;
    if (i == thread_nb-1){
      th_d.end = th_d.start+d_per_thread_remainder;
    }
    else{
      th_d.end = th_d.start+d_per_thread;
    }
    th_d.minibatch = minibatch;
    th_d.net = net;
    th_d.sum = 0;
    *(th_data_arr+i) = th_d;
  }
  return th_data_arr;
}
