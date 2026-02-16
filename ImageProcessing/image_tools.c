#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_surface.h>
#include <math.h>

#include "image_tools.h"

#define BRIGHTNESS_FACTOR 2.0

void PutPixel32_nolock(SDL_Surface *surface, int x, int y, Uint8 r, Uint8 g,
                       Uint8 b) {
  Uint8 *pixel = (Uint8 *)surface->pixels;
  pixel += (y * surface->pitch) + (x * surface->format->BytesPerPixel);
  *((Uint32 *)pixel) = SDL_MapRGB(surface->format, r, g, b);
}

void get_luminance(Uint32 pixel, SDL_PixelFormat *format, Uint8 *r, Uint8 *g,
                   Uint8 *b) {
  SDL_GetRGB(pixel, format, r, g, b);
  double lum_r = *r / 255.0;
  double lum_g = *g / 255.0;
  double lum_b = *b / 255.0;
  double luminance = 0.299 * lum_r + 0.587 * lum_g + 0.114 * lum_b;

  double res = 0;
  if (luminance <= 0.0031308)
    res = 12.92 * luminance;
  else
    res = 1.055 * pow(luminance, 1 / 2.4) - 0.055;
  *g = *b = *r = 255 * res;
}

void gray_level(SDL_Surface *surface) {
  SDL_LockSurface(surface);
  SDL_PixelFormat *format = surface->format;

  for (int j = 0; j < surface->h; j++) {
    Uint8 *row = (Uint8 *)surface->pixels + j * surface->pitch;
    for (int i = 0; i < surface->w; i++) {
      Uint8 r, g, b;
      Uint32 *pixel = (Uint32 *)(row + i * surface->format->BytesPerPixel);
      get_luminance(*pixel, format, &r, &g, &b);
      PutPixel32_nolock(surface, i, j, r, g, b);
    }
  }
  SDL_UnlockSurface(surface);
}

void black_and_white(SDL_Surface *surface) {
  SDL_LockSurface(surface);
  SDL_PixelFormat *format = surface->format;

  for (int j = 0; j < surface->h; j++) {
    Uint8 *row = (Uint8 *)surface->pixels + j * surface->pitch;
    for (int i = 0; i < surface->w; i++) {
      Uint32 *pixel = (Uint32 *)(row + i * surface->format->BytesPerPixel);
      Uint8 r, g, b, n;
      SDL_GetRGB(*pixel, format, &r, &g, &b);
      n = (r + g + b) / 3 > 170 ? 255 : 0;
      PutPixel32_nolock(surface, i, j, n, n, n);
    }
  }

  SDL_UnlockSurface(surface);
}

void negatif(SDL_Surface *surface) {
  SDL_LockSurface(surface);
  SDL_PixelFormat *format = surface->format;

  for (int j = 0; j < surface->h; j++) {
    Uint8 *row = (Uint8 *)surface->pixels + j * surface->pitch;
    for (int i = 0; i < surface->w; i++) {
      Uint32 *pixel = (Uint32 *)(row + i * surface->format->BytesPerPixel);
      Uint8 r, g, b;
      SDL_GetRGB(*pixel, format, &r, &g, &b);
      PutPixel32_nolock(surface, i, j, 255 - r, 255 - g, 255 - b);
    }
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
  SDL_LockSurface(surface);
  SDL_PixelFormat *format = surface->format;

  for (int j = 0; j < surface->h; j++) {
    Uint8 *row = (Uint8 *)surface->pixels + j * surface->pitch;
    for (int i = 0; i < surface->w; i++) {
      Uint32 *pixel = (Uint32 *)(row + i * surface->format->BytesPerPixel);
      Uint8 r, g, b;
      SDL_GetRGB(*pixel, format, &r, &g, &b);

      r = f(r, n);
      g = f(g, n);
      b = f(b, n);

      PutPixel32_nolock(surface, i, j, r, g, b);
    }
  }

  SDL_UnlockSurface(surface);
}
void increase_contrast(SDL_Surface *surface) { contrast(surface, 2); }
void decrease_contrast(SDL_Surface *surface) { contrast(surface, -2); }

void change_brightness(SDL_Surface *surface, double n) {
  SDL_LockSurface(surface);
  SDL_PixelFormat *format = surface->format;

  for (int j = 0; j < surface->h; j++) {
    Uint8 *row = surface->pixels + j * surface->pitch;
    for (int i = 0; i < surface->w; i++) {
      Uint32 *pixel = (Uint32 *)(row + i * surface->format->BytesPerPixel);
      Uint8 r, g, b;
      SDL_GetRGB(*pixel, format, &r, &g, &b);

      r = (Uint8)(255 * SDL_pow((double)r / 255.0, n));
      g = (Uint8)(255 * SDL_pow((double)g / 255.0, n));
      b = (Uint8)(255 * SDL_pow((double)b / 255.0, n));

      PutPixel32_nolock(surface, i, j, r, g, b);
    }
  }

  SDL_UnlockSurface(surface);
}

void increase_brightness(SDL_Surface *surface) {
  change_brightness(surface, 2);
}
void decrease_brightness(SDL_Surface *surface) {
  change_brightness(surface, 0.5);
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

  Uint32 *pixels = surface->pixels;
  int w = surface->w, h = surface->h;

  SDL_LockSurface(surface);
  SDL_LockSurface(copy);

  for (int i = 0; i < h; i++)
    for (int j = 0; j < w; j++)
      pixels[i * w + j] = gaussian(copy, i, j);

  SDL_UnlockSurface(surface);
  SDL_UnlockSurface(copy);
  SDL_FreeSurface(copy);
}

void median(SDL_Surface *surface) {

  int width = surface->w, height = surface->h, kernel_size = 2;
  Uint32 *pixels = surface->pixels;
  Uint32 *output_pixels = malloc(width * height * sizeof(Uint32));
  int half_kernel = kernel_size / 2;

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      Uint8 r[25], g[25], b[25];
      int count = 0;

      for (int dy = -half_kernel; dy <= half_kernel; dy++) {
        for (int dx = -half_kernel; dx <= half_kernel; dx++) {
          int nx = x + dx;
          int ny = y + dy;

          if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
            Uint32 pixel = pixels[ny * width + nx];
            SDL_GetRGB(pixel, surface->format, &r[count], &g[count], &b[count]);
            count++;
          }
        }
      }

      for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
          if (r[j] > r[j + 1]) {
            Uint8 temp_r = r[j];
            r[j] = r[j + 1];
            r[j + 1] = temp_r;
          }
          if (g[j] > g[j + 1]) {
            Uint8 temp_g = g[j];
            g[j] = g[j + 1];
            g[j + 1] = temp_g;
          }
          if (b[j] > b[j + 1]) {
            Uint8 temp_b = b[j];
            b[j] = b[j + 1];
            b[j + 1] = temp_b;
          }
        }
      }

      int median_index = count / 2;
      Uint32 new_pixel = SDL_MapRGB(surface->format, r[median_index],
                                    g[median_index], b[median_index]);
      output_pixels[y * width + x] = new_pixel;
    }
  }

  memcpy(pixels, output_pixels, width * height * sizeof(Uint32));
  free(output_pixels);
}
