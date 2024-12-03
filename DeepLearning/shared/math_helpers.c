#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "math_helpers.h"


double sigmoid(double z){
  return 1/(1+exp(-z));
}

double sigmoid_derivative(double z){
  return sigmoid(z)*(1-sigmoid(z));
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
