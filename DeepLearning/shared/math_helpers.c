#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "math_helpers.h"

#define LEAKY_CONSTANT 1e-2

// old
double _sigmoid(double z){
  return 1/(1+exp(-z));
}

double _sigmoid_derivative(double z){
  return _sigmoid(z)*(1-_sigmoid(z));
}

// he
double random_normal(double mean, double stddev) {
    // Box-Muller transform for generating normal distribution
    double u1 = ((double) rand() / RAND_MAX);
    double u2 = ((double) rand() / RAND_MAX);
    double z0 = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);
    return mean + z0 * stddev;
}

// new
double leaky_ReLu(double z){
  if (z > 0){
    return z;
  }
  else{
    return LEAKY_CONSTANT*z;
  }
}
double leaky_ReLu_derivative(double z){
  if (z>0){
    return 1;
  }
  else{
    return LEAKY_CONSTANT;
  }
}
void softmax(double* z, size_t size, double* a){
  double Z = 0;
  for (size_t i=0; i<size; i++){
    Z+=exp(*(z+i));
  }
  for (size_t i=0; i<size; i++){
    *(a+i) = exp(*(z+i))/Z;
  }
}

double double_rand( double min, double max )
{
  double x = (double)rand()/(double)RAND_MAX;
  return min + x * (max-min);
}

double* rand_normal_double_array(double mean, double stddev, size_t size){
  double* arr = calloc(size, sizeof(double));
  for(size_t i=0; i<size; i++){
    *(arr+i) = random_normal(mean, stddev);
  }
  return arr;
}

double* he_weight_array(size_t size){
  return rand_normal_double_array(0, sqrt(2.0 / size), size);
}
