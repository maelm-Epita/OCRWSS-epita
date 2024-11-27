#pragma once
#include <stddef.h>

char* sze_arr_to_str(size_t* arr, size_t size);
char* float_arr_to_str(float* arr, size_t size);
void str_arr_clear(char* arr, size_t size);
void print_float_arr(float* arr, size_t size);
float* sub_arr(float* arr1, float* arr2, size_t size);
float norm(float* arr, size_t size);
float av_arr(float* arr, size_t size);
void shuffle_arr(float *array, size_t n);
