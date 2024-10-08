#ifndef IMAGE_TOOLS_H
#define IMAGE_TOOLS_H

#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_surface.h>
#include <math.h>

struct pixel {
  int r, g, b;
  int nb;
};

void get_luminance(Uint32 pixel, SDL_PixelFormat *format, Uint8 *r, Uint8 *g,
                   Uint8 *b);

void gray_level(SDL_Surface *surface);

void binary_gray_level(SDL_Surface *surface);

void gray_test(SDL_Surface *surface);

void negatif(SDL_Surface *surface);

void contrast(SDL_Surface *surface, double n);

void increase_brightness(SDL_Surface *surface, double n);

void decrease_brightness(SDL_Surface *surface, double n);


Uint32 gaussian(SDL_Surface *surface, int i, int j);

void gauss(SDL_Surface *surface);


void dying_filter(SDL_Surface *surface, int n);

#endif
