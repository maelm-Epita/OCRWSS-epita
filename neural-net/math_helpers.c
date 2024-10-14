#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "math_helpers.h"

float sigmoid(float z){
  return 1/(1+exp(-z));
}
// FALSE, here i am doing the partial derivative with rrespect to vector w but it  should be with respect to a single w (pass in the index for example)
// redo the whole function to be more readable
float partial_derivative_output(float(*f)(float*, float*, float, size_t), float* var1, float var2, float* var3, size_t var4, char to_d){
  // if we're doing the partial derivative of the output with respect to weights 
  if (to_d == 1){
    float* nw = calloc(var4, sizeof(float));
    for (size_t i=0; i<var4;i++){
      *(nw+i) = *(var1+i)+EPS;
    }
    float res = (f(var3, nw, var2, var4) - f(var3, var1, var2, var4))/EPS;
    free(nw);
    return res;
  }
  // if we're doing the partial derivative of the output with respect to bias
  else if (to_d == 2){
    return (f(var3, var1, var2+EPS, var4)-f(var3,var1,var2,var4))/EPS;
  }
  else{
    printf("not a valid variable to derive with respect to");
    exit(EXIT_FAILURE);
  }
}
