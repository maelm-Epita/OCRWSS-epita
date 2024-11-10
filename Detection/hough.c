#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_pixels.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
// #include <gtk/gtk.h>

#include "hough.h"
// #include "../Rotate/rotate.h"

int lowestLineY = 0;
double lowestLineAngle = 0;

double **getBrightness(SDL_Surface *surface) {

  Uint8 r, g, b;
  Uint32 *pixels = surface->pixels;

  int height = surface->h, width = surface->w;
  double **matrix = malloc(height * sizeof(double *));

  for (int y = 0; y < height; y++) {
    matrix[y] = malloc(width * sizeof(double));

    for (int x = 0; x < width; x++) {
      SDL_GetRGB(pixels[y * width + x], surface->format, &r, &g, &b);
      matrix[y][x] = 0.299 * r + 0.587 * g + 0.114 * b;
    }
  }

  return matrix;
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

void detectLines(SDL_Surface *surface) {
  int **matrix = detectEdges(surface);

  if (matrix == NULL) {
    puts("Aieaieaieaie on a un petit probleme");
    return;
  }

  int threshold = 0, width = surface->w - 2, height = surface->h - 2,
      size = (int)sqrt(pow((double)height, 2) + pow((double)width, 2)) + 1;

  int **accumulatorArray = malloc(size * 2 * sizeof(int *));
  for (int i = 0; i < size * 2; i++) {
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
        if (++accumulatorArray[rho + size][theta] > threshold) {
          threshold = accumulatorArray[rho + size][theta];
        }
      }
    }
  }

  threshold -= threshold / 5;

  for (int rho = 0; rho < size * 2; rho++) {
    for (int theta = 0; theta < 180; theta++) {

      if (accumulatorArray[rho][theta] > threshold) {
        drawLine(surface, (double)(rho - size), (double)theta, height, width);
      }
    }
    free(accumulatorArray[rho]);
  }
  free(accumulatorArray);

  /*
    if (SDL_SaveBMP(surface, filepath) != 0) {
        printf("Image saving error: %s\n", SDL_GetError());
    }
    else {
        printf("Image processed successfully and saved as '%s'.\n", filepath);
    }


    if ((int)lowestLineAngle % 90 != 0) {
        if (lowestLineAngle > 90) {
            rotate(filepath, 90 - lowestLineAngle);
        }
        else {
            rotate(filepath, lowestLineAngle);
        }
    }
  */
}

void drawLine(SDL_Surface *surface, double rho, double theta, int h, int w) {

  double x1, y1, x2, y2;

  if (theta == 90) {
    x1 = 0;
    x2 = w;
    y1 = y2 = rho;
  } else if (theta == 0) {
    x1 = x2 = rho;
    y1 = 0;
    y2 = h;
  } else {

    double rad_theta = theta * (double)M_PI / 180.0;
    double cos_theta = cos(rad_theta);
    double sin_theta = sin(rad_theta);

    y1 = rho / sin_theta;
    if (y1 < 0) {
      y1 = 0;
      x1 = rho / cos_theta;
    } else if (y1 > h) {
      y1 = h;
      x1 = (rho - h * sin_theta) / cos_theta;
    } else {
      x1 = 0;
    }

    y2 = (rho - w * cos_theta) / sin_theta;
    if (y2 < 0) {
      y2 = 0;
      x2 = rho / cos_theta;
    } else if (y2 > h) {
      y2 = h;
      x2 = (rho - h * sin_theta) / cos_theta;
    } else {
      x2 = w;
    }
  }

  if (y1 + y2 > lowestLineY) {
    lowestLineY = y1 + y2;
    lowestLineAngle = theta;
  }

  drawLineOnSurface(surface, x1, y1, x2, y2);
}

void drawLineOnSurface(SDL_Surface *surface, int x1, int y1, int x2, int y2) {
  int dx = abs(x2 - x1);
  int dy = abs(y2 - y1);
  int sx = (x1 < x2) ? 1 : -1;
  int sy = (y1 < y2) ? 1 : -1;
  int err = dx - dy;
  Uint32 *pixels = surface->pixels;

  while (x1 != x2 || y1 != y2) {
    pixels[y1 * surface->w + x1] = SDL_MapRGB(surface->format, 255, 0, 0);
    // put_pixel(surface, x1, y1);

    int e2 = err * 2;
    if (e2 > -dy) {
      err -= dy;
      x1 += sx;
    }
    if (e2 < dx) {
      err += dx;
      y1 += sy;
    }
  }
}

void put_pixel(SDL_Surface *surface, int x, int y) {
  if (x >= 0 && x < surface->w && y >= 0 && y < surface->h) {
    Uint32 *pixels = (Uint32 *)surface->pixels;
    pixels[y * surface->w + x] = SDL_MapRGB(surface->format, 255, 0, 0);
  }
}
