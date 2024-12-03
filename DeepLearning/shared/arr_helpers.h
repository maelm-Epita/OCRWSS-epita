#pragma once
#include <stddef.h>

char* sze_arr_to_str(size_t* arr, size_t size);
char* double_arr_to_str(double* arr, size_t size);
void str_arr_clear(char* arr, size_t size);
void print_double_arr(double* arr, size_t size);
double* sub_arr(double* arr1, double* arr2, size_t size);
double norm(double* arr, size_t size);
double av_arr(double* arr, size_t size);
void shuffle_arr(double *array, size_t n);
void free_double_matrix(double **matrix, size_t size);
void average_matrix(double **in_matrix, double* out_vecto, size_t i_max, size_t j_max);
