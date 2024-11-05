#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_surface.h>
#include <err.h>
#include <stdio.h>

#include "image_matrix.h"

#define LIMIT 230

struct image_matrix image_to_matrix(SDL_Surface *surface) {
  struct image_matrix res;
  res.w = surface->w;
  res.h = surface->h;
  res.data = calloc(res.w * res.h, sizeof(int));
  if (res.data == NULL)
    errx(1, "Malloc return NULL");

  Uint32 *pixels = (Uint32 *)surface->pixels;
  size_t size = surface->w * surface->h;
  SDL_LockSurface(surface);
  SDL_PixelFormat *format = surface->format;

  for (size_t i = 0; i < size; i++) {
    Uint8 r;
    Uint32 pixel = *(pixels + i);
    SDL_GetRGB(pixel, format, &r, &r, &r);

    res.data[i] = SDL_MapRGB(surface->format, r, r, r);
    //res.data[i] = r > LIMIT ? 0 : 1;
  }

  SDL_UnlockSurface(surface);
  return res;
}

void print_matrix(struct image_matrix *mat) {
  for (int j = 0; j < mat->h; j++) {
    for (int i = 0; i < mat->w; i++) {
      if (mat->data[mat->w * j + i] == 0)
        putchar('.');
      else
        putchar('+');
    }
    putchar('\n');
  }
}

SDL_Surface *matrix_to_surface(struct image_matrix *mat, SDL_Surface *surface){
  (void)mat;
  return SDL_CreateRGBSurfaceWithFormat(0, mat->w, mat->h, 32, surface->format->format);
}
