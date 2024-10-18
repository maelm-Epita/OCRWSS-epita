#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <err.h>
#include <stdlib.h>

#include "image_tools.h"
#include "image_matrix.h"

#define STEP_ANGLE 5

double angle = 0;

SDL_Window *create_window(char *title, int w, int h) {
  int status = SDL_Init(SDL_INIT_VIDEO);
  if (status)
    return NULL;
  return SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                          w, h, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
}

int main(int argc, char *argv[]) {
  if (argc < 2)
    errx(EXIT_FAILURE, "use image: ./image <path to bmp");

  struct image_matrix mat;

  SDL_Surface *image = IMG_Load(argv[1]);
  if (image == NULL)
    errx(EXIT_FAILURE, "%s", SDL_GetError());

  int max_side = image->h > image->w ? image->h : image->w;
  SDL_Window *window = create_window("OCR", max_side, max_side);
  if (window == NULL)
    errx(EXIT_FAILURE, "%s", SDL_GetError());

  SDL_Renderer *renderer = SDL_CreateRenderer(
      window, 1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (renderer == NULL)
    errx(EXIT_FAILURE, "%s", SDL_GetError());

  int run = 1;
  while (run) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT)
        run = 0;
      else if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.scancode) {
        case SDL_SCANCODE_O:
          angle -= STEP_ANGLE;
          break;
        case SDL_SCANCODE_P:
          angle += STEP_ANGLE;
          break;
        case SDL_SCANCODE_G:
          gray_level(image);
          break;
        case SDL_SCANCODE_B:
          binary_gray_level(image);
          break;
        case SDL_SCANCODE_H:
          gray_test(image);
          break;
        case SDL_SCANCODE_N:
          negatif(image);
          break;
        case SDL_SCANCODE_X:
          contrast(image, 2.0);
          break;
        case SDL_SCANCODE_Z:
          contrast(image, -2.0);
          break;
        case SDL_SCANCODE_W:
          increase_brightness(image, 2);
          break;
        case SDL_SCANCODE_Q:
          increase_brightness(image, 0.5);
          break;
        case SDL_SCANCODE_F:
          gauss(image);
          break;
        case SDL_SCANCODE_D:
          dying_filter(image, 3);
          break;
        case SDL_SCANCODE_R:
          gaussian(image, image->w / 2, image->h / 5);
          break;
        case SDL_SCANCODE_M:
          mat = image_to_matrix(image);
          image = matrix_to_surface(&mat, image);
          // print_matrix(&mat);
          break;
        default:
          break;
        }
      }
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, image);

    if (SDL_RenderCopyEx(renderer, texture, NULL, NULL, angle, NULL,
                         SDL_FLIP_NONE))
      errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_RenderPresent(renderer);
  }

  SDL_FreeSurface(image);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return EXIT_SUCCESS;
}