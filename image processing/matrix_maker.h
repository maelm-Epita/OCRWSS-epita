#ifndef MATRIX_MAKER_H
#define MATRIX_MAKER_H

#include <SDL2/SDL.h>

struct image_matrix{
  int w, h;
  int *data;
};

struct image_matrix image_to_matrix(SDL_Surface *surface);

void print_matrix(struct image_matrix *mat);

SDL_Surface *matrix_to_surface(struct image_matrix *mat, SDL_Surface *surface);

#endif
