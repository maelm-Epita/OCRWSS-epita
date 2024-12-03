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
double* rand_double_array(double min, double max, size_t size){
  double* arr = calloc(size, sizeof(double));
  for(size_t i=0; i<size; i++){
    *(arr+i) = double_rand(min, max);
  }
  return arr;
}
