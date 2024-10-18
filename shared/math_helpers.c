#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "math_helpers.h"


float sigmoid(float z){
  return 1/(1+exp(-z));
}

float float_rand( float min, float max )
{
  float x = (float)rand()/(float)RAND_MAX;
  return min + x * (max-min);
}
float* rand_float_array(float min, float max, size_t size){
  float* arr = calloc(size, sizeof(float));
  for(size_t i=0; i<size; i++){
    *(arr+i) = float_rand(min, max);
  }
  return arr;
}
