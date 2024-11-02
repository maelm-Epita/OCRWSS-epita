#ifndef DETCTION_H
#define DETCTION_H

#include <SDL2/SDL.h>

typedef struct{
  int x, y;
} point;

typedef struct{
  point min;
  point max;
} block;

static int BLOCK_SIZE = 100;
static double THRESHOLD = 0.1;


void lines(SDL_Surface* img, block *block);

void columns(SDL_Surface *img, block *block, int b_line, int e_line);

#endif
