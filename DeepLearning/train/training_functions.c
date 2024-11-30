#include "../neural-net/Network.h"
#include "../shared/arr_helpers.h"
#include "training_data.h"
#include "training_functions.h"
#include "../threading/threading.h"
#include <err.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define ARBITRARY_MIN_INITIALIZER 100
#define EPS 1e-3;

float Cost(struct Network net, struct training_data data) {
  // the output size is the size of the last layer
  size_t outputsize = *(net.layersizes + net.layernb - 1);
  float *output = feedforward(net, data.inputs);
  float *diff = sub_arr(output, data.expected_output, outputsize);
  float cost = pow(norm(diff, outputsize), 2) / 2;
  free(diff);
  free(output);
  return cost;
}
float *get_Costs(struct Network net, struct training_set minibatch) {
  float *costs = calloc(minibatch.data_number, sizeof(float));
  for (size_t i = 0; i < minibatch.data_number; i++) {
    *(costs + i) = Cost(net, *(minibatch.data + i));
  }
  return costs;
}
float av_Cost(struct Network net, struct training_set minibatch, size_t thread_nb) {
  float av_cost = -1;
  if (thread_nb <= 1){
    float *costs = get_Costs(net, minibatch);
    av_cost = av_arr(costs, minibatch.data_number);
    free(costs);
  }
  else{
    float sum = 0;
    int e;
    struct thread_data *th_data_arr = create_thread_data_array(thread_nb, &minibatch, &net);
    for (size_t t=0; t<thread_nb; t++){
      pthread_t thr;
      e = pthread_create(&thr, NULL, worker, th_data_arr+t);
      if (e!=0){
        errx(EXIT_FAILURE, "Failed to create thread %lu\n", t);
      }
      (th_data_arr+t)->sys_id = thr;
    }
    for (size_t t=0; t<thread_nb; t++){
      pthread_join((th_data_arr+t)->sys_id, NULL);
      sum += (th_data_arr+t)->sum;
    }
    av_cost = sum/minibatch.data_number;
  }
  return av_cost;
}

float av_Cost_PDW(struct Network net, struct Neuron *neuron, size_t windex,
                  struct training_set minibatch, size_t thread_nb) {
  float av_cost = av_Cost(net, minibatch, thread_nb);
  float w = *(neuron->weights + windex);
  *(neuron->weights + windex) = w + EPS;
  float d_av_cost = av_Cost(net, minibatch, thread_nb);
  *(neuron->weights + windex) = w;
  return (d_av_cost - av_cost) / EPS;
}
float av_Cost_PDB(struct Network net, struct Neuron *neuron,
                  struct training_set minibatch, size_t thread_nb) {
  float av_cost = av_Cost(net, minibatch, thread_nb);
  float b = neuron->bias;
  neuron->bias = b + EPS;
  float d_av_cost = av_Cost(net, minibatch, thread_nb);
  neuron->bias = b;
  return (d_av_cost - av_cost) / EPS;
}

float back_propagate(struct Network *net, struct training_set minibatch,
                     float rate, size_t thread_nb) {
  for (size_t l = 0; l < net->layernb; l++) {
    struct Layer *clayer = net->layers + l;
    for (size_t n = 0; n < *(net->layersizes + l); n++) {
      struct Neuron *cneuron = clayer->neurons + n;
      for (size_t w = 0; w < cneuron->inputsize; w++) {
        float acpd = av_Cost_PDW(*net, cneuron, w, minibatch, thread_nb);
        *(cneuron->weights + w) = *(cneuron->weights + w) - rate * acpd;
      }
      float acpd = av_Cost_PDB(*net, cneuron, minibatch, thread_nb);
      cneuron->bias = cneuron->bias - rate * acpd;
    }
  }
  float av_cost = av_Cost(*net, minibatch, thread_nb);
  return av_cost;
}

float train(struct Network *net, struct training_set set, double rate,
            size_t minibatch_size, size_t epochs, char* model_name,
            int backprop_nb, size_t thread_nb, char print_b) {
  if (print_b){
    printf("----------------------\n");
    printf("TRAINING NEURAL NETWORK\n");
    printf("----------------------\n");
  }
  size_t it = backprop_nb;
  clock_t start, end;
  for (size_t i = 0; i <= epochs; i++) {
    float av_cost = 0;
    if (print_b){
      printf("EPOCH %lu\n", i);
      printf("Training.....\n");
    }
    struct minibatch_set mini_set = create_minibatch_set(set, minibatch_size);
    for (size_t j = 0; j < mini_set.minibatch_number && it != 0; j++) {
      struct training_set curr_minibatch = *(mini_set.mini_batches + j);
      if (print_b){
        printf("Mini-batch %lu - Back propagation...\n", j);
      }
      float cost;
      start = clock();
      cost = back_propagate(net, curr_minibatch, rate, thread_nb);
      av_cost += cost;
      end = clock();
      if (print_b){
        printf("Mini batch took : %f seconds\n", (double)(end-start)/CLOCKS_PER_SEC);  
        printf("Mini batch cost : %f\n", cost);
        printf("Saving network...\n");
      }
      save_network(model_name, *net);
      it--;
    }
    av_cost /= mini_set.minibatch_number;
    if (print_b){
      printf("EPOCH %lu finished, Average cost was : %f\n", i, av_cost);
    }
    free_minibatch_set(mini_set);
  }
  if (print_b){
    printf("END OF TRAINING...\n");
    printf("---------------------\n");
  }
  float final_av_cost = av_Cost(*net, set, thread_nb);
  if (print_b){
    printf("Final average cost of the network on all training data : %f\n",
           final_av_cost);
  }
  return final_av_cost;
}

float train_fork(struct Network base_net, struct training_set set, double rate, 
                 size_t minibatch_size, size_t epochs, size_t backprop_nb, 
                 size_t nb_children, size_t thread_nb, char* model_name){
  // allocate for cost pipes and pipe res pipe
  int (*costfds)[2] = calloc(nb_children, sizeof(int[2]));
  int resfd[2];
  if (pipe(resfd) < 0) {
    errx(EXIT_FAILURE, "pipe failed for res\n");
  }
  for (size_t i = 0; i < nb_children; i++) {
    // delay for random
    sleep(5);
    // initialize the child's pipe
    if (pipe(costfds[i]) < 0) {
      errx(EXIT_FAILURE, "Failed to pipe child %lu\n", i);
    }
    // forking
    int id = fork();
    if (id < 0) {
      errx(EXIT_FAILURE, "Failed to fork\n");
    }
    // child code
    else if (id == 0) {
      printf("Child %lu: Training\n", i);
      int pid = getpid();
      // close unused pipe ends
      close(costfds[i][0]); // read close
      close(resfd[1]); // write close
      // copy model network for the child, fill
      struct Network net = {base_net.inputsize, base_net.layernb, base_net.layersizes, NULL};
      fill_network(&net);
      // train net of the child 
      float cost = train(&net, set, rate, minibatch_size, epochs, model_name, backprop_nb, thread_nb, 0);
      printf("Finished training child %lu: Cost is %.6f, Pid is %d\n", i, cost, pid);
      // sending results to father thread
      if (write(costfds[i][1], &cost, sizeof(cost)) < (long)sizeof(cost)){
        errx(EXIT_FAILURE, "Failed to write all cost\n");
      }
      if (write(costfds[i][1], &pid, sizeof(pid)) < (long)sizeof(cost)){
        errx(EXIT_FAILURE, "Failed to write all pid\n");
      }
      // we can send eof through the pipe by closing the writing end
      close(costfds[i][1]);
      // now we will read the result sent by the father
      // all children will be blocked on the read instruction until
      // the father sends bytes through the pipe
      int selected;
      printf("Child %lu: Waiting for response from parent...\n", i);
      if (read(resfd[0], &selected, sizeof(int)) < (long)sizeof(int)){
        errx(EXIT_FAILURE, "Failed to read all pid from parent\n");
      }
      // the child saves or not depending on the instruction from the father
      if (pid == selected) {
        printf("Child %lu, pid %d: saved network\n", i, pid);
        save_network(model_name, net);
      }
      printf("Child %lu: exiting\n", i);
      // cleaning up
      free_network(&net);
      close(resfd[0]);
      // exiting
      exit(EXIT_SUCCESS);
    }
  }
  // parent code
  // close unused ends of res pipe
  close(resfd[0]); // read close
  // the father process should be waiting for execution of all children until they write their results
  // reading from each child's write pipe to know each pid and cost in order
  // to find the minimum cost's child pid
  // we do not need to worry about timing the reads because read will block execution until it receives data
  float min_cost = ARBITRARY_MIN_INITIALIZER;
  int min_pid;
  float temp_cost;
  int temp_pid;
  printf("Parent: waiting for all children to finish...\n");
  for (size_t i = 0; i < nb_children; i++) {
    // we will close the unused end of the cost pipe of each child
    close(costfds[i][1]);
    if (read(costfds[i][0], &temp_cost, sizeof(float)) < (long)sizeof(float)){
        errx(EXIT_FAILURE, "Failed to read all cost from child %lu\n", i);
    }
    if (read(costfds[i][0], &temp_pid, sizeof(int)) < (long)sizeof(int)){
        errx(EXIT_FAILURE, "Failed to read all pid from child %lu\n", i);
    }
    if (temp_cost < min_cost) {
      min_cost = temp_cost;
      min_pid = temp_pid;
    }
    printf("Parent: Successfully read cost from child %lu\n", i);
    // after reading from the child's cost pipe, we can close the reading end
    close(costfds[i][0]);
  }
  printf("Found minimum cost in child with pid : %d, sending response to children\n", min_pid);
  // the father process will write the correct pid once for each child, so they can all read it
  for (size_t i = 0; i < nb_children; i++) {
    if (write(resfd[1], &min_pid, sizeof(int)) < (long)sizeof(int)){
      errx(EXIT_FAILURE, "Failed to read all pid from child %lu\n", i);
    }
  }
  // it can then close the writing end so that the children reach EOF
  close(resfd[1]);
  // then the father will wait until all children have exited
  printf("Waiting for all children to exit...\n");
  for (size_t i = 0; i < nb_children; i++) {
    wait(NULL);
  }
  printf("--------------------\n");
  printf("Training finished, the minimal cost found was : %f, in the child with pid : %d\n", min_cost, min_pid);
  return min_cost;
}
