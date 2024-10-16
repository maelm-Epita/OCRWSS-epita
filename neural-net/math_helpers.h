#pragma once

static const float EPS = 1e-8;
float sigmoid(float z);
// partial derivative of 4 arguments function f (output function)
// args : 0:function, 1:var1, 2:var2, 3:var3, 4:var4 3:partial derivative with respect to var #(1 or 2)
float partial_derivative_output(float(*f)(float*, float*, float, size_t), float* var1, float var2, float* var3, size_t var4, char to_d);
// random float in range
float float_rand( float min, float max );
float* rand_float_array(float min, float max, size_t size);
float* rand_float_array(float min, float max, size_t size);
