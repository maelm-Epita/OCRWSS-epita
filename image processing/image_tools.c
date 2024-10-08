#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_surface.h>
#include <math.h>

#include "image_tools.h"

#define BRIGHTNESS_FACTOR 2.0

void get_luminance(Uint32 pixel, SDL_PixelFormat *format, Uint8 *r, Uint8 *g,
                   Uint8 *b) {
  SDL_GetRGB(pixel, format, r, g, b);
  double lum_r = *r / 255.0;
  double lum_g = *g / 255.0;
  double lum_b = *b / 255.0;
  double luminance = 0.2126 * lum_r + 0.7152 * lum_g + 0.0722 * lum_b;

  double res;
  if (luminance <= 0.0031308)
    res = 12.92 * luminance;
  else
    res = 1.055 * pow(luminance, 1 / 2.4) - 0.055;
  *g = *b = *r = 255 * res;
}

void gray_level(SDL_Surface *surface) {
  Uint32 *pixels = (Uint32 *)surface->pixels;
  size_t size = surface->w * surface->h;
  SDL_LockSurface(surface);
  SDL_PixelFormat *format = surface->format;

  for (size_t i = 0; i < size; i++) {
    Uint8 r, g, b;
    Uint32 *pixel = pixels + i;
    get_luminance(*pixel, format, &r, &g, &b);
    *pixel = SDL_MapRGB(format, r, g, b);
  }

  SDL_UnlockSurface(surface);
}

void binary_gray_level(SDL_Surface *surface) {
  Uint32 *pixels = (Uint32 *)surface->pixels;
  size_t size = surface->w * surface->h;
  SDL_LockSurface(surface);
  SDL_PixelFormat *format = surface->format;

  for (size_t i = 0; i < size; i++) {
    Uint8 r, g, b, n;
    SDL_GetRGB(pixels[i], format, &r, &g, &b);

    n = (r + g + b) / 3 > 170 ? 255 : 0;

    pixels[i] = SDL_MapRGB(format, n, n, n);
  }

  SDL_UnlockSurface(surface);
}

void gray_test(SDL_Surface *surface) {
  Uint32 *pixels = (Uint32 *)surface->pixels;
  size_t size = surface->w * surface->h;
  SDL_LockSurface(surface);
  SDL_PixelFormat *format = surface->format;

  for (size_t i = 0; i < size; i++) {
    Uint8 r, g, b, n;
    SDL_GetRGB(pixels[i], format, &r, &g, &b);

    n = (r + g + b) / 3;

    pixels[i] = SDL_MapRGB(format, n, n, n);
  }

  SDL_UnlockSurface(surface);
}

void negatif(SDL_Surface *surface) {
  Uint32 *pixels = (Uint32 *)surface->pixels;
  size_t size = surface->w * surface->h;
  SDL_LockSurface(surface);
  SDL_PixelFormat *format = surface->format;

  for (size_t i = 0; i < size; i++) {
    Uint8 r, g, b;
    SDL_GetRGB(pixels[i], format, &r, &g, &b);

    pixels[i] = SDL_MapRGB(format, 255 - r, 255 - g, 255 - b);
  }

  SDL_UnlockSurface(surface);
}

Uint8 f(Uint8 c, double n) {
  if (c <= 255 / 2)
    return (Uint8)((255 / 2.0) * SDL_pow((double)2 * c / 255, n));
  else
    return 255 - f(255 - c, n);
}
void contrast(SDL_Surface *surface, double n) {
  Uint32 *pixels = (Uint32 *)surface->pixels;
  size_t size = surface->w * surface->h;
  SDL_LockSurface(surface);
  SDL_PixelFormat *format = surface->format;

  for (size_t i = 0; i < size; i++) {
    Uint8 r, g, b;
    SDL_GetRGB(pixels[i], format, &r, &g, &b);

    r = f(r, n);
    g = f(g, n);
    b = f(b, n);

    pixels[i] = SDL_MapRGB(format, r, g, b);
  }

  SDL_UnlockSurface(surface);
}

void increase_brightness(SDL_Surface *surface, double n) {
  Uint32 *pixels = (Uint32 *)surface->pixels;
  size_t size = surface->w * surface->h;
  SDL_LockSurface(surface);
  SDL_PixelFormat *format = surface->format;

  for (size_t i = 0; i < size; i++) {
    Uint8 r, g, b;
    SDL_GetRGB(pixels[i], format, &r, &g, &b);

    r = (Uint8)(255 * SDL_pow((double)r / 255.0, n));
    g = (Uint8)(255 * SDL_pow((double)g / 255.0, n));
    b = (Uint8)(255 * SDL_pow((double)b / 255.0, n));
    /*
    r = r + n > 255 ? 255 : 0;
    g = g + n > 255 ? 255 : 0;
    b = b + n > 255 ? 255 : 0;
    */

    pixels[i] = SDL_MapRGB(format, r, g, b);
  }

  SDL_UnlockSurface(surface);
}

void decrease_brightness(SDL_Surface *surface, double n) {
  increase_brightness(surface, 1 / n);
}

Uint32 average(SDL_Surface *surface, int i, int j, int n) {
  const int initial_h = SDL_max(i - n, 0);
  const int initial_w = SDL_max(j - n, 0);
  const int final_h = SDL_min(i + n, surface->h - 1);
  const int final_w = SDL_min(j + n, surface->w - 1);
  const int nb_pixel = ((final_h - initial_h + 1) * (final_w - initial_w + 1));
  const Uint32 *p = surface->pixels;

  Uint32 sum_r = 0, sum_g = 0, sum_b = 0;
  Uint8 r, g, b;

  for (int x = initial_h; x <= final_h; x++)
    for (int y = initial_w; y <= final_w; y++) {
      SDL_GetRGB(p[x * surface->w + y], surface->format, &r, &g, &b);
      sum_r += r;
      sum_g += g;
      sum_b += b;
    }

  return SDL_MapRGB(surface->format, sum_r / nb_pixel, sum_g / nb_pixel,
                    sum_b / nb_pixel);
}

#define N 1
#define SIGMA 1.0
static double kernel[2 * N + 1][2 * N + 1];

void generate_gaussian_kernel(void) {
  double sum = 0.0;
  int size = 2 * N + 1;

  for (int x = -N; x <= N; x++) {
    for (int y = -N; y <= N; y++) {
      kernel[x + N][y + N] = exp(-(x * x + y * y) / (2 * SIGMA * SIGMA));
      sum += kernel[x + N][y + N];
    }
  }

  // Normalize Kernel
  for (int x = 0; x < size; x++) {
    for (int y = 0; y < size; y++) {
      kernel[x][y] /= sum;
    }
  }
}

Uint32 gaussian(SDL_Surface *surface, int i, int j) {

  const int initial_h = SDL_max(i - N, 0);
  const int initial_w = SDL_max(j - N, 0);
  const int final_h = SDL_min(i + N, surface->h - 1);
  const int final_w = SDL_min(j + N, surface->w - 1);
  const Uint32 *p = surface->pixels;

  double sum_r = 0.0f, sum_g = 0.0f, sum_b = 0.0f;
  double sum_weights = 0.0;
  Uint8 r, g, b;
  SDL_GetRGB(p[i * surface->w + j], surface->format, &r, &g, &b);

  if (r + g + b > 250) {
    for (int x = initial_h; x <= final_h; x++)
      for (int y = initial_w; y <= final_w; y++) {
        SDL_GetRGB(p[x * surface->w + y], surface->format, &r, &g, &b);
        double k_value = kernel[x - i + N][y - j + N];
        sum_r += r * k_value;
        sum_g += g * k_value;
        sum_b += b * k_value;
        sum_weights += k_value;
      }
  }

  return SDL_MapRGB(surface->format, sum_r / sum_weights, sum_g / sum_weights,
                    sum_b / sum_weights);
}

void gauss(SDL_Surface *surface) {
  generate_gaussian_kernel();
  SDL_Surface *copy = SDL_ConvertSurface(surface, surface->format, 0);

  Uint32 *pixels = (Uint32 *)surface->pixels;
  int w = surface->w, h = surface->h;

  SDL_LockSurface(surface);
  SDL_LockSurface(copy);

  for (int i = 0; i < h; i++)
    for (int j = 0; j < w; j++)
      pixels[i * w + j] = gaussian(copy, i, j);
  // pixels[i * w + j] = average(copy, i, j, 3);

  SDL_UnlockSurface(surface);
  SDL_UnlockSurface(copy);
  SDL_FreeSurface(copy);
}

Uint32 dying(SDL_Surface *surface, int i, int j, int n) {
  const int initial_h = SDL_max(i - n, 0);
  const int initial_w = SDL_max(j - n, 0);
  const int final_h = SDL_min(i + n, surface->h - 1);
  const int final_w = SDL_min(j + n, surface->w - 1);
  const Uint32 *p = surface->pixels;

  struct pixel *pixels =
      calloc((n * 2 + 1) * (n * 2 + 1), sizeof(struct pixel));
  if (pixels == NULL)
    perror("calloc()");
  int nb_pixels = 0;
  Uint8 r, g, b;

  for (int x = initial_h; x <= final_h; x++)
    for (int y = initial_w; y <= final_w; y++) {
      SDL_GetRGB(p[x * surface->w + y], surface->format, &r, &g, &b);

      int find = 0, i = 0;
      while (!find && i < nb_pixels) {
        if (pixels[i].r == r && pixels[i].g == g && pixels[i].b == b) {
          find = 1;
          pixels[i].nb++;
        }
        i++;
      }
      if (!find) {
        pixels[i] = (struct pixel){r, g, b, 1};
        nb_pixels++;
      }
    }

  int i_max = 0, nb_max = pixels[0].nb;
  for (int i = 1; i < nb_pixels; i++) {
    if (pixels[i].nb > nb_max) {
      nb_max = pixels[i].nb;
      i_max = i;
    }
  }
  r = pixels[i_max].r, g = pixels[i_max].g, b = pixels[i_max].b;
  // free(pixels);
  return SDL_MapRGB(surface->format, r, g, b);
}

void dying_filter(SDL_Surface *surface, int n) {
  SDL_Surface *copy = SDL_ConvertSurface(surface, surface->format, 0);

  Uint32 *pixels = (Uint32 *)surface->pixels;
  int w = surface->w, h = surface->h;

  SDL_LockSurface(surface);
  SDL_LockSurface(copy);

  for (int i = 0; i < h; i++)
    for (int j = 0; j < w; j++)
      pixels[i * w + j] = dying(copy, i, j, n);

  SDL_UnlockSurface(surface);
  SDL_UnlockSurface(copy);
  SDL_FreeSurface(copy);
}
