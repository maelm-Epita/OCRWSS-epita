#pragma once
#include <stddef.h>

double leaky_ReLu(double z);
double leaky_ReLu_derivative(double z);
void softmax(double* z, size_t size, double* a);
// random double in range
double double_rand( double min, double max );
double* rand_double_array(double min, double max, size_t size);
double* rand_double_array(double min, double max, size_t size);
