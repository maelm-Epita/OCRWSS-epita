#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Variables Globales
//
// eps => epsilone is a very small quantity that we will add to our parameters
// to calculate their partial derivatives
//
// rate => the rate is a small value representing the speed at which our neural
// network learns, the larger the rate the faster the nn learns, be careful if
// the latter is too large the nn will produce the opposite effect

static float eps = 1e-1;
static float rate = 1.f;

// XOR = (x OR y) AND (x NAND y)

// the sigmoid is a function which for all x belonging to the reals,
// returns a float between 0 and 1
float sigmoidf(float x) { return 1.f / (1.f + expf(-x)); }

// rand_float() generates a random float
float rand_float() { return (float)rand() / (float)RAND_MAX; }

typedef struct {
  float or [3];
  float nand[3];
  float and[3];
} Xor;

// the forward() function makes the xor from the OR, NAND and AND gates
float forward(Xor m, float x1, float x2) {
  float a = sigmoidf(m.or [0] * x1 + m.or [1] * x2 + m.or [2]);
  float b = sigmoidf(m.nand[0] * x1 + m.nand[1] * x2 + m.nand[2]);
  return sigmoidf(a * m.and[0] + b * m.and[1] + m.and[2]);
}

// sample is a float array with 3 elements, it will help us to represent the
// expected values with different xor parameters
typedef float sample[3];

// the xor_train is an array of sample where we are givving the parameters in
// the first to indexes and the result we are willing to have in the third one
sample xor_train[] = {
    {0, 0, 1},
    {0, 1, 0},
    {1, 0, 0},
    {1, 1, 1},
};

// *train is a pointer to our xor_train
sample *train = xor_train;

// we are initialising the lengh of our train at 4 because we have 4 tests
size_t train_count = 4;

// the cost function returns the diffrence of the ouput of our nn and the
// willing output, it serve us to track the training, the more cost tends
// to 0, the better
float cost(Xor m) {
  float result = 0.0f;
  for (size_t i = 0; i < train_count; i++) {
    float x1 = train[i][0];
    float x2 = train[i][1];
    float y = forward(m, x1, x2);
    float d = y - train[i][2];
    result += d * d;
  }
  result /= train_count;
  return result;
}

// rand_xor initialise all weights and biais of a Xor matrice with random floats
Xor rand_xor() {
  Xor m;
  for (size_t i = 0; i < 3; i++) {
    m.or[i] = rand_float();
  }
  for (size_t i = 0; i < 3; i++) {
    m.nand[i] = rand_float();
  }
  for (size_t i = 0; i < 3; i++) {
    m.and[i] = rand_float();
  }
  return m;
}

// print_xor prints all the elements inside a given Xor
void print_xor(Xor m) {
  printf("or_w1 = %f\n", m.or [0]);
  printf("or_w2 = %f\n", m.or [1]);
  printf("or_b = %f\n", m.or [2]);
  printf("nand_w1 = %f\n", m.and[0]);
  printf("nand_w2 = %f\n", m.and[1]);
  printf("nand_b = %f\n", m.and[2]);
  printf("and_w1 = %f\n", m.nand[0]);
  printf("and_w2 = %f\n", m.nand[1]);
  printf("and_b = %f\n", m.nand[2]);
}

// diff is doing all the gradiants
Xor diff(Xor m) {
  Xor g;
  float c = cost(m);
  // utilisation d un variable pour save pour ne pas avoir d erreur de
  // soustraction
  float saved;

  // pour chaque poids et biais on reevallue les valeurs grace a des derivés
  // partielles
  for (size_t i = 0; i < 3; i++) {
    saved = m.or [i];
    m.or [i] += eps;
    g.or [i] = (cost(m) - c) / eps;
    m.or [i] = saved;
  }
  for (size_t i = 0; i < 3; i++) {
    saved = m.nand[i];
    m.nand[i] += eps;
    g.nand[i] = (cost(m) - c) / eps;
    m.nand[i] = saved;
  }
  for (size_t i = 0; i < 3; i++) {
    saved = m.and[i];
    m.and[i] += eps;
    g.and[i] = (cost(m) - c) / eps;
    m.and[i] = saved;
  }
  return g;
}

// train takes 2 xor and substract the second one to the first one and
// apply the choosen rate in order to train the nn and have better
// weights and biais
Xor sub_rate(Xor m, Xor g) {
  for (size_t i = 0; i < 3; i++) {
    m.or [i] -= g.or [i] * rate;
  }
  for (size_t i = 0; i < 3; i++) {
    m.nand[i] -= g.nand[i] * rate;
  }
  for (size_t i = 0; i < 3; i++) {
    m.and[i] -= g.and[i] * rate;
  }
  return m;
}

int main() {
  srand(0);

  Xor m = rand_xor();
  //print_xor(m);
  float c = cost(m);
  //printf("cost = %f\n", c);
  printf("%f\n", c);
  for (size_t i = 0; i < 100000; i++) {
    Xor g = diff(m);
    m = sub_rate(m, g);
    printf("[gen %lu] le cout est de : %f\n", i, cost(m));
    //printf("cost = %f\n", cost(m));
  }
  printf("-----------------------\n");
  print_xor(m);
  printf("-----------------------\n");
  for (size_t i = 0; i < 2; i++) {
    for (size_t j = 0; j < 2; j++) {
      printf("%zu | %zu = %f\n", i, j, forward(m, i, j));
    }
  }

  return 0;
}
