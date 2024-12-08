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

void detect_grid_and_word_list(SDL_Surface *surface, int *res);


void lines(SDL_Surface* img, block *block);

void columns(SDL_Surface *img, block *block, int b_line, int e_line);

#endif
