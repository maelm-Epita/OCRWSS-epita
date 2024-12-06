#include "resize.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <err.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 28
#define HEIGHT 28 

Uint8 get_gray(SDL_Surface *src, size_t x, size_t y) {
  if (x >= (size_t)src->w || y >= (size_t)src->h) {
    errx(EXIT_FAILURE, "the pixel is outside the borders of the image");
  }
  printf("pixel lu est : (%lu,%lu)\n", x, y);
  Uint8 r, g, b;
  Uint32 *pix = (Uint32 *)src->pixels;
  SDL_GetRGB(pix[y * (src->pitch / 4) + x], src->format, &r, &g, &b);
  Uint8 gray_val = (r + g + b) / 3;
  return gray_val;
}
/*
void set_gray(SDL_Surface *src, size_t x, size_t y, Uint8 val){
  Uint8 *pix = (Uint8 *)src->pixels;
  pix[y * src->pitch + x] = val;
}
*/
void set_gray(SDL_Surface *dst, size_t x, size_t y, Uint8 val) {
  Uint32 pix = SDL_MapRGB(dst->format, val, val, val);
  SDL_LockSurface(dst);
  Uint32 *pixels = (Uint32*)dst->pixels; 
  pixels[y * WIDTH + x] = pix;
  SDL_UnlockSurface(dst);
}

double cubic_inter(double pos1, double pos2, double pos3, double pos4,
                   double x) {
  double a = -0.5 * pos1 + 1.5 * pos2 - 1.5 * pos3 + 0.5 * pos4;
  double b = pos1 - 2.5 * pos2 + 2 * pos3 - 0.5 * pos4;
  double c = -0.5 * pos1 + 0.5 * pos2;
  double d = pos2;
  return a * (x * x * x) + b * (x * x) + c * x + d;
}

double bicubic_inter(double mat[4][4], double x, double y) {
  double cols[4];
  for (size_t i = 0; i < 4; i++) {
    cols[i] = cubic_inter(mat[i][0], mat[i][1], mat[i][2], mat[i][3], x);
  }
  return cubic_inter(cols[0], cols[1], cols[2], cols[3], y);
}

SDL_Surface *resize(SDL_Surface *src) {

  // toutes les images seront possible, meme vals dans r g b pour avoir du gris

  /*
  if (src->format->BitsPerPixel != 8) {
    errx(EXIT_FAILURE, "not a gray scale image");
  }

  SDL_Surface *dst = SDL_CreateRGBSurfaceWithFormat(0, WIDTH, HEIGHT, 8,
  SDL_PIXELFORMAT_INDEX8);
   */
  Uint32 pixels[WIDTH * HEIGHT];
  double fact_scale_hor = (double)src->w / WIDTH;
  double fact_scale_ver = (double)src->h / HEIGHT;
  SDL_Surface *dst = SDL_CreateRGBSurfaceWithFormat(0, WIDTH, HEIGHT, src->format->BitsPerPixel, src->format->format);
  if (dst == NULL) {
    errx(EXIT_FAILURE, "could not create the dst SDL_Surface");
  }
  for (size_t y = 0; y < HEIGHT; y++) {
    for (size_t x = 0; x < WIDTH; x++) {
      double srcx = x * fact_scale_hor;
      double srcy = y * fact_scale_ver;
      double x0 = floor(srcx);
      double y0 = floor(srcy);

      double mat[4][4];
      printf("gonna see for mat of center : (%f,%f)\n", x0, y0);
      printf("-------------------------------------\n");
      for (size_t j = 0; j < 4; j++) {
        for (size_t i = 0; i < 4; i++) {
          int px = fmin(fmax(x0 + i, 0), src->w - 1);
          int py = fmin(fmax(y0 + j, 0), src->h - 1);
          if (px < 0 || px >= src->w || py < 0 || py >= src->h) {
            errx(EXIT_FAILURE, "Erreur d'accès à la matrice : px=%d, py=%d\n",
                 px, py);
          }
          mat[j][i] = get_gray(src, px, py);
        }
      }

      printf("\n\n");
      double tx = srcx - floor(srcx);
      double ty = srcy - floor(srcy);

      double temp_pix = bicubic_inter(mat, tx, ty);
      Uint8 pix = (Uint8)fmin(fmax(temp_pix, 0), 255);
      set_gray(dst, x, y, pix);
    }
  }
  

  printf("done\n");
  return dst;
}

// Fonction principale
int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <path to image>\n", argv[0]);
    return EXIT_FAILURE;
  }

  // Initialisation de SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    errx(EXIT_FAILURE, "Could not initialize SDL: %s", SDL_GetError());
  }

  // Chargement de l'image
  SDL_Surface *image = IMG_Load(argv[1]);
  if (image == NULL) {
    errx(EXIT_FAILURE, "Could not load image: %s", SDL_GetError());
  }
  SDL_Surface *resized_image = resize(image);
  if (resized_image == NULL) {
    errx(EXIT_FAILURE, "resize returned a NULL surface");
  }

  // Vérifiez les propriétés de la surface
  printf("Resized surface info: width=%d, height=%d, pitch=%d, format=%s\n",
         resized_image->w, resized_image->h, resized_image->pitch,
         SDL_GetPixelFormatName(resized_image->format->format));


  // Sauvegardez l'image
  if (SDL_SaveBMP(resized_image, "resized.bmp") != 0) {
    errx(EXIT_FAILURE, "Could not save BMP: %s", SDL_GetError());
  }

  // Libérez les surfaces
  SDL_FreeSurface(resized_image);
  return EXIT_SUCCESS;
}
