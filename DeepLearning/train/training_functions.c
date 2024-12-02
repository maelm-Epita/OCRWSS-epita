#include "../neural-net/Network.h"
#include "../shared/arr_helpers.h"
#include "training_data.h"
#include "training_functions.h"
#include "../threading/threading.h"
#include "../shared/math_helpers.h"
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
  float *output = feedforward(net, data.inputs, NULL, NULL);
  float sub;
  float cost = 0;
  for (size_t i=0; i<outputsize; i++){
    sub = *(output+i) - *(data.expected_output+i);
    cost+=pow(sub, 2);
  }
  free(output);
  return cost;
}

float pd_Cost_of_activation(float activation, float expected){
  return 2 * (activation-expected);
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

void back_propagate(struct Network *net, struct training_data data, float* d_grad_w, float* d_grad_b){
  // INITIALIZE the z matrix and the a matrix
  // z = w*i+b
  // a = activation(z)
  // index_b will help us write to the grad vector of biases
  // index_w will help us write to the grad vector of weights
  float** z_mat = calloc(net->layernb, sizeof(float*));
  float** a_mat = calloc(net->layernb, sizeof(float*));
  size_t index_b = 0;
  size_t index_w = 0;
  // FORWARD pass through the network, filling our matrixes
  feedforward(*net, data.inputs, z_mat, a_mat);
  // BACKPROP
  float* NextLayer_pd_Cost_Act = NULL;
  for (int l=net->layernb-1; l>=0; l--){
    size_t layersize = *(net->layersizes+l);
    // Grab the partial derivative of the cost with respect to the activation of each neuron
    // => how much does the cost change as the activation (output) of each specific neuron changes
    float* pd_Cost_Act = calloc(layersize, sizeof(float));
    // If we are on the last layer, the formula is specially defined as the derivative of our cost function in relation to weights only
    if ((size_t)l==net->layernb-1){
      for (size_t n=0; n<layersize; n++){
        *(pd_Cost_Act+n) = pd_Cost_of_activation(a_mat[l][n], *(data.expected_output+n));
      }
    }
    // Otherwise, it is defined recursively in relation to the next layer after
    else{
      for (size_t n=0; n<layersize; n++){
        float pdn = 0;
        // Sum impact of the change in activation of the neuron over all the neurons in the next layer
        for (size_t k=0; k<*(net->layersizes+l+1); k++){
          // the weight linking the activation of neuron n of this layer to the neuron k of the next layer
          // => the nth weight of the kth neuron of the l+1th layer
          float w_k_j = *(((net->layers+l+1)->neurons+k)->weights+n);
          // Hard to understand formula
          pdn += w_k_j * sigmoid_derivative(z_mat[l+1][k]) * NextLayer_pd_Cost_Act[k];
        }
        *(pd_Cost_Act+n) = pdn;
      }
    }
    // Now that we have the partial derivative of the cost with respect to the activation of each neuron in the layer
    // We can compute the partial derivative of the cost with respect to each weight or bias in the layer
    // Meaning we have the grad for a single layer
    for (size_t n=0; n<layersize; n++){
      float sig_d_z_l_n = sigmoid_derivative(z_mat[l][n]);
      *(d_grad_b+index_b) = sig_d_z_l_n * pd_Cost_Act[n];
      index_b++;
      // For this term, we use the previous layer's activation
      // If we are on the first layer, this will be the input of the network
      if (l==0){
        for (size_t k=0; k<data.input_number; k++){
          *(d_grad_w+index_w) = data.inputs[k] * sig_d_z_l_n * pd_Cost_Act[n];
          index_w++;
        }
      }
      else{
        for (size_t k=0; k<*(net->layersizes+l-1); k++){
          *(d_grad_w+index_w) = a_mat[l-1][k] * sig_d_z_l_n * pd_Cost_Act[n];
          index_w++;
        }
      }
    }

    // We can discard the previously calculated pd_cost_act we do not need anymore
    if (NextLayer_pd_Cost_Act != NULL){
      free(NextLayer_pd_Cost_Act);
    }
    // We go to the previous layer thus currentlayer pdcostact becomes nextlayer pdcostact
    NextLayer_pd_Cost_Act = pd_Cost_Act;
  }
  // CLEAN up
  free(NextLayer_pd_Cost_Act);
  free_float_matrix(z_mat, net->layernb);
  free_float_matrix(a_mat, net->layernb);
}

void back_propagate_minibatch(struct Network *net, struct training_set minibatch, float* grad_w, float* grad_b, size_t total_weight_nb, size_t total_bias_nb){
  // Initialize matrix of all our gradients of weights and matrix of all gradients of biases
  // for single training datas; given by back_propagate
  float** d_grad_ws = calloc(minibatch.data_number, sizeof(float*));
  float** d_grad_bs = calloc(minibatch.data_number, sizeof(float*));
  // Fill the matrixes
  for (size_t i=0; i<minibatch.data_number; i++){
    float* d_grad_w = calloc(total_weight_nb, sizeof(float));
    float* d_grad_b = calloc(total_bias_nb, sizeof(float));
    back_propagate(net, *(minibatch.data+i), d_grad_w, d_grad_b);
    *(d_grad_ws+i) = d_grad_w;
    *(d_grad_bs+i) = d_grad_b;
  }
  // Find the average of each weight change and bias change (average gradient) over the minibatch
  average_matrix(d_grad_ws, grad_w, minibatch.data_number, total_weight_nb);
  average_matrix(d_grad_bs, grad_b, minibatch.data_number, total_bias_nb);
  // Clean up
  free_float_matrix(d_grad_ws, minibatch.data_number);
  free_float_matrix(d_grad_bs, minibatch.data_number);
}

void update_minibatch(struct Network *net, struct training_set minibatch, float rate, size_t total_weight_nb, size_t total_bias_nb){
  float* grad_w = calloc(total_weight_nb, sizeof(float));
  float* grad_b = calloc(total_bias_nb, sizeof(float));
  size_t index_w = 0;
  size_t index_b = 0;
  back_propagate_minibatch(net, minibatch, grad_w, grad_b, total_weight_nb, total_bias_nb);
  for (int l=net->layernb-1; l>=0; l--){
    size_t layersize = *(net->layersizes+l);
    // apply gradient for neuron
    for (size_t n=0; n<layersize; n++){
      struct Neuron *neuron = ((net->layers+l)->neurons+n);
      for (size_t w=0; w<neuron->inputsize; w++){
        *(neuron->weights+w) -= rate*grad_w[index_w];
        index_w++;
      }
      neuron->bias -= rate*grad_b[index_b];
    }
  }
  free(grad_w);
  free(grad_b);
}

float train(struct Network *net, struct training_set set, double rate,
            size_t minibatch_size, size_t epochs, char* model_name,
            int backprop_nb, size_t thread_nb, char print_b) {
  if (print_b){
    printf("----------------------\n");
    printf("TRAINING NEURAL NETWORK\n");
    printf("----------------------\n");
  }
  // INITIALIZE USEFUL VARIABLES FOR THE ENTIRE TRAINING
  size_t it = backprop_nb;
  clock_t start, end;
  size_t total_weight_nb;
  size_t total_bias_nb;
  calc_weight_bias_amount(*net, &total_weight_nb, &total_bias_nb);
  // EACH EPOCH 
  for (size_t i = 0; i <= epochs; i++) {
    float av_cost = 0;
    if (print_b){
      printf("EPOCH %lu\n", i);
      printf("Training.....\n");
    }
    // CREATE THE MINIBATCH SET
    struct minibatch_set mini_set = create_minibatch_set(set, minibatch_size);
    // GO THROUGH THE ENTIRE SET DOING 1 BACKPROP PER BATCH
    for (size_t j = 0; j < mini_set.minibatch_number && it != 0; j++) {
      struct training_set curr_minibatch = *(mini_set.mini_batches + j);
      if (print_b){
        printf("Mini-batch %lu - Back propagation...\n", j);
      }
      start = clock();
      update_minibatch(net, curr_minibatch, rate, total_weight_nb, total_bias_nb);
      float cost = Cost(*net, *(curr_minibatch.data));
      end = clock();
      if (print_b){
        printf("Mini batch took : %f seconds\n", (double)(end-start)/CLOCKS_PER_SEC);  
        printf("Current cost of network over a random training example of the batch: %f\n", cost);
        printf("Saving network...\n");
      }
      save_network(model_name, *net);
      it--;
    }
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
