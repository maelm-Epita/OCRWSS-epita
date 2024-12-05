#pragma once
#include <pthread.h>
#include "../train/training_data.h"
#include "../neural-net/Network.h"

// pass all necessary information to perform gradrient descent on weights
struct thread_data{
  long id;
  pthread_t sys_id;
  float sum;
  size_t start;
  size_t end;
  struct training_set *minibatch;
  struct Network *net;
};

// the worker function back_propagates on a thread from the weight data->start to data->end in a neuron
void *worker(void *arg);
// creates the correct thread data structures to create threads and pass as arguments to the function
struct thread_data* create_thread_data_array(size_t thread_nb,
                                             struct training_set *minibatch, struct Network *net);
