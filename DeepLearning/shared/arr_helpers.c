#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "arr_helpers.h"

char* sze_arr_to_str(size_t* arr, size_t size){
  char* str = calloc(9*size, sizeof(char));
  size_t str_size = 0;
  for (size_t i=0; i<size; i++){
    str_size += sprintf(str+str_size, "%lu ", *(arr+i));
  }
  *(str+str_size-1) = 0;
  str = realloc(str, str_size);
  return str;
}
char* float_arr_to_str(float* arr, size_t size){
  char* str = calloc(16*size, sizeof(char));
  size_t str_size = 0;
  for (size_t i=0; i<size; i++){
    str_size += sprintf(str+str_size, "%f ", *(arr+i));
  }
  *(str+str_size-1) = 0;
  str = realloc(str, str_size);
  return str;
}

void str_arr_clear(char* arr, size_t size){
  for (size_t i=0; i<size; i++){
    *(arr+i)=0;
  }
}

void print_float_arr(float* arr, size_t size){
  printf("{ ");
  for (size_t i=0; i<size; i++){
    printf("%f ", *(arr+i));
  }
  printf("}\n");
}

float* sub_arr(float* arr1, float* arr2, size_t size){
  float* narr = calloc(size, sizeof(float));
  for (size_t i=0; i<size; i++){
    *(narr+i) = *(arr1+i)-*(arr2+i);
  }
  return narr;
}
float norm(float* arr, size_t size){
  float n = 0;
  for (size_t i=0; i<size; i++){
    n += pow(*(arr+i),2);
  }
  return sqrt(n);
}
float av_arr(float* arr, size_t size){
  float arr_sum = 0;
  for (size_t i=0; i<size; i++){
    arr_sum += *(arr+i);
  }
  return arr_sum/size;
}

void free_float_matrix(float **matrix, size_t size){
  for (size_t i=0; i<size; i++){
    free(*(matrix+i));
  }
  free(matrix);
}

void average_matrix(float **in_matrix, float* out_vect, size_t i_max, size_t j_max){
  // j = weight
  for (size_t j=0; j<j_max; j++){
    float av=0;
    // i = minibatch 
    for (size_t i=0; i<i_max; i++){
      av+=in_matrix[i][j];
    }
    av/=i_max;
    *(out_vect+j)=av;
  }
}

