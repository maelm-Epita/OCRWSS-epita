#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <err.h>

#include "image_rotate.h"

Uint32 *copy_pixels(SDL_Surface *s) {
  Uint32 *res = calloc(s->w * s->h, sizeof(Uint32));
  Uint32 *pixels = s->pixels;
  size_t size = s->w * s->h;
  if (res == NULL)
    errx(1, "calloc():");

  for (size_t i = 0; i < size; i++)
    res[i] = pixels[i];

  return res;
}

SDL_Surface *rotate_90_right(SDL_Surface *surface) {
  int w = surface->w, h = surface->h;

  SDL_Surface *res = SDL_CreateRGBSurface(
      0, h, w, surface->format->BitsPerPixel, surface->format->Rmask,
      surface->format->Gmask, surface->format->Bmask, surface->format->Amask);

  if (!res)
    errx(1, "%s", SDL_GetError());

  Uint32 *pixels = copy_pixels(surface);
  Uint32 *p = res->pixels;
  SDL_LockSurface(surface);
  SDL_LockSurface(res);

  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      p[j * h + (h - 1 - i)] = pixels[i * w + j];
    }
  }

  SDL_UnlockSurface(res);
  SDL_UnlockSurface(surface);
  free(pixels);
  SDL_FreeSurface(surface);

  return res;
}

SDL_Surface *rotate_90_left(SDL_Surface *surface) {
  int w = surface->w, h = surface->h;

  SDL_Surface *res = SDL_CreateRGBSurface(
      0, h, w, surface->format->BitsPerPixel, surface->format->Rmask,
      surface->format->Gmask, surface->format->Bmask, surface->format->Amask);

  if (!res)
    errx(1, "%s", SDL_GetError());

  Uint32 *pixels = copy_pixels(surface);
  Uint32 *p = res->pixels;
  SDL_LockSurface(surface);
  SDL_LockSurface(res);

  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      p[(w - 1 - j) * h + i] = pixels[i * w + j];
    }
  }

  SDL_UnlockSurface(res);
  SDL_UnlockSurface(surface);
  free(pixels);
  SDL_FreeSurface(surface);

  return res;
}

SDL_Surface *rotate_180(SDL_Surface *surface) {
  int w = surface->w, h = surface->h;
  size_t size = w * h;

  Uint32 *pixels = copy_pixels(surface);
  Uint32 *p = surface->pixels;
  SDL_LockSurface(surface);

  for (size_t i = 0; i < size; i++)
    p[size - 1 - i] = pixels[i];

  SDL_UnlockSurface(surface);
  free(pixels);

  return surface;
}
