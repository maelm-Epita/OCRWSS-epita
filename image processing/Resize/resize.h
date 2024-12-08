#ifndef RESIZE_H
#define RESIZE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

Uint8 get_gray(SDL_Surface *src, size_t x, size_t y);
void set_gray(SDL_Surface *dst, size_t x, size_t y, Uint8 val);
double cubic_inter(double pos1, double pos2, double pos3, double pos4, double x);
double bicubic_inter(double mat[4][4], double x, double y);
SDL_Surface *resize(SDL_Surface *src);
char *file_name_creator(char *name);

#endif

