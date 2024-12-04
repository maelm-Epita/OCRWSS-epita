#ifndef IMAGE_TOOLS_H
#define IMAGE_TOOLS_H

#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_surface.h>
#include <math.h>

void get_luminance(Uint32 pixel, SDL_PixelFormat *format, Uint8 *r, Uint8 *g,
                   Uint8 *b);

void gray_level(SDL_Surface *surface);

void black_and_white(SDL_Surface *surface);

void negatif(SDL_Surface *surface);

void increase_contrast(SDL_Surface *surface);

void decrease_contrast(SDL_Surface *surface);

void increase_brightness(SDL_Surface *surface);

void decrease_brightness(SDL_Surface *surface);

Uint32 gaussian(SDL_Surface *surface, int i, int j);

void gauss(SDL_Surface *surface);

#endif
