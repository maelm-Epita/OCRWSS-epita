#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cairo/cairo.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "image_tools.h"

double current_angle = 0;

double **getBrightness(SDL_Surface *surface) {

  Uint8 r, g, b;
  Uint32 *pixels = (Uint32 *)surface->pixels;

  int height = surface->h, width = surface->w;
  double **res = malloc(height * sizeof(double *));

  for (int y = 0; y < height; y++) {
    res[y] = malloc(width * sizeof(double));

    for (int x = 0; x < width; x++) {
      SDL_GetRGB(pixels[y * width + x], surface->format, &r, &g, &b);
      res[y][x] = 0.299 * r + 0.587 * g + 0.114 * b;
    }
  }

  return res;
}

int **detectEdges(SDL_Surface *surface) {

  int height = surface->h, width = surface->w;
  int **res = malloc((height - 2) * sizeof(int *));
  double **matrix = getBrightness(surface);

  if (matrix == NULL)
    return NULL;

  for (int y = 1; y < height - 1; y++) {
    res[y - 1] = malloc((width - 2) * sizeof(int));

    for (int x = 1; x < width - 1; x++) {
      res[y - 1][x - 1] =
          (int)sqrt(pow(-1 * matrix[y - 1][x - 1] - 2 * matrix[y][x - 1] -
                            1 * matrix[y + 1][x - 1] + matrix[y - 1][x + 1] +
                            2 * matrix[y][x + 1] + matrix[y + 1][x + 1],
                        2) +
                    pow(-1 * matrix[y - 1][x - 1] - 2 * matrix[y - 1][x] -
                            1 * matrix[y - 1][x + 1] + matrix[y + 1][x - 1] +
                            2 * matrix[y + 1][x] + matrix[y + 1][x + 1],
                        2)) /
          4;
    }
  }

  for (int i = 0; i < height; i++) {
    free(matrix[i]);
  }
  free(matrix);

  return res;
}

int **detectLines(SDL_Surface *surface, int *size, int *threshold) {

  int **matrix = detectEdges(surface);
  if (matrix == NULL) {
    puts("Error with edges detection");
    SDL_Quit();
    return NULL;
  }

  int width = surface->w - 2, height = surface->h - 2;
  *size = (int)sqrt(pow((double)height, 2) + pow((double)width, 2)) + 1;

  int **accumulatorArray = malloc((*size) * 2 * sizeof(int *));
  for (int i = 0; i < (*size) * 2; i++) {
    accumulatorArray[i] = malloc(180 * sizeof(int));
    for (int j = 0; j < 180; j++) {
      accumulatorArray[i][j] = 0;
    }
  }

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {

      if (matrix[y][x] <= 50)
        continue;

      int rho;
      for (int theta = 0; theta < 180; theta++) {

        rho = x * cos(theta * M_PI / 180) + y * sin(theta * M_PI / 180);
        if (++accumulatorArray[rho + *size][theta] > *threshold) {
          *threshold = accumulatorArray[rho + *size][theta];
        }
      }
    }
  }

  return accumulatorArray;
}

SDL_Surface *rotate(SDL_Surface *original_surface, double angle) {

  current_angle = 0;
  SDL_Window *window =
      SDL_CreateWindow("Rotation", 10000, SDL_WINDOWPOS_CENTERED,
                       original_surface->w, original_surface->h, 0);
  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_TARGETTEXTURE);

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);

  SDL_Texture *texture =
      SDL_CreateTextureFromSurface(renderer, original_surface);

  SDL_Rect destRect;
  destRect.w = original_surface->w;
  destRect.h = original_surface->h;

  SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(
      0, destRect.w, destRect.h, 32, SDL_PIXELFORMAT_ARGB8888);

  SDL_RenderCopyEx(renderer, texture, NULL, NULL, angle, NULL, SDL_FLIP_NONE);
  SDL_RenderReadPixels(renderer, NULL, surface->format->format, surface->pixels,
                       surface->pitch);

  SDL_FreeSurface(original_surface);
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  return surface;
}

/*
void update_image(double angle) {
    current_angle = angle;
    int width = gdk_pixbuf_get_width(original_pixbuf);
    int height = gdk_pixbuf_get_height(original_pixbuf);
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
width, height); cairo_t *cr = cairo_create(surface);
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    cairo_paint(cr);
    cairo_translate(cr, width / 2.0, height / 2.0);
    cairo_rotate(cr, current_angle * G_PI / 180.0);
    cairo_translate(cr, -width / 2.0, -height / 2.0);
    gdk_cairo_set_source_pixbuf(cr, original_pixbuf, 0, 0);
    cairo_paint(cr);
    GdkPixbuf *rotated_pixbuf = gdk_pixbuf_get_from_surface(surface, 0, 0,
width, height); gtk_image_set_from_pixbuf(GTK_IMAGE(original_image),
rotated_pixbuf); cairo_destroy(cr); cairo_surface_destroy(surface);
    g_object_unref(rotated_pixbuf);
}
*/

SDL_Surface *autoRotate(SDL_Surface *surface) {
  int size = 0, threshold = 0;
  int **accumulatorArray = detectLines(surface, &size, &threshold);

  threshold /= 2;
  int maxTheta = 0, angle = 0;

  for (int theta = 0; theta < 180; theta++) {
    int currentTheta = 0;

    for (int rho = 0; rho < size * 2; rho++) {
      if (accumulatorArray[rho][theta] > threshold) {
        currentTheta++;
      }
    }

    if (currentTheta > maxTheta) {
      maxTheta = currentTheta;
      angle = theta;
    }
  }

  for (int rho = 0; rho < size * 2; rho++)
    free(accumulatorArray[rho]);
  free(accumulatorArray);
  // SDL_FreeSurface(surface);

  if ((int)angle % 90 != 0) {
    if (angle > 90)
      return rotate(surface, 90 - angle);
    else
      return rotate(surface, -angle);
  }

  return surface;
}
