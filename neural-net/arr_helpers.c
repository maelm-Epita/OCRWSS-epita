
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

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
