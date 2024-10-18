#ifndef IMAGE_ROTATE_H
#define IMAGE_ROTATE_H

#include <SDL2/SDL_surface.h>

SDL_Surface *rotate_90_right(SDL_Surface *surface);

SDL_Surface *rotate_90_left(SDL_Surface *surface);

SDL_Surface *rotate_180(SDL_Surface *surface);

#endif
