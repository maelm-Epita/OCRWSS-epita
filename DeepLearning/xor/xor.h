#ifndef XOR_H
#define XOR_H

float sigmoidf(float x);
float rand_float();
typedef struct {} Xor;
float forward(Xor m, float x1, float x2);
typedef float sample[3];
float cost(Xor m);
Xor rand_xor();
void print_xor(Xor m);
Xor diff(Xor m);
Xor sub_rate(Xor m, Xor g);

#endif
