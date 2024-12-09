#ifndef DETCTION_H
#define DETCTION_H

#include <SDL2/SDL.h>

#include "detect.h"

typedef struct{
  point min;
  point max;
} block;

void detect_grid_and_word_list(const SDL_Surface *surface, int *res);


void lines(SDL_Surface* img, block *block);

void columns(SDL_Surface *img, block *block, int b_line, int e_line);

#endif
