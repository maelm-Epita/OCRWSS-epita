#include <SDL2/SDL_image.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_surface.h>
#include <err.h>
#include <unistd.h>

#define OUTPUT_SIZE 26
#define DATA_WIDTH 28
#define DATA_HEIGHT 28

double *image_to_input(char *path) {
  SDL_Surface *img = IMG_Load(path);
  if (img == NULL)
    errx(EXIT_FAILURE, "Could not open image");

  SDL_LockSurface(img);
  SDL_PixelFormat *format = img->format;
  Uint32 *pixels = img->pixels;
  size_t size = img->w * img->h;
  double *input = calloc(size, sizeof(double));

  if (input == NULL)
    errx(EXIT_FAILURE, "could not create input array");

  for (size_t i = 0; i < size; i++) {
    Uint8 r, g, b;
    Uint32 pixel = pixels[i];
    SDL_GetRGB(pixel, format, &r, &g, &b);
    double greyscale = (double)(r + g + b) / 3.0;
    // squash input to range [0;1]
    input[i] = greyscale / 255;
  }
  SDL_FreeSurface(img);
  return input;
}

void input_to_image(double* input) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0){
    exit(EXIT_FAILURE);
  }
	SDL_Window *win = SDL_CreateWindow("Input display", 100, 100, 28, 28, 0);
	SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
  SDL_RenderClear(renderer);
  for (size_t i=0; i<DATA_HEIGHT; i++){
    for (size_t j=0; j<DATA_WIDTH; j++){
      size_t col = *(input+i*28+j) * 255;
      SDL_SetRenderDrawColor(renderer, col, col, col, 255);
      SDL_RenderDrawPoint(renderer, j, i);
    }
  }
  SDL_RenderPresent(renderer);
  sleep(2);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(win);
  SDL_Quit();
}

char output_to_prediction(double* output){
  size_t max = 0;
  for (size_t i=0; i<OUTPUT_SIZE; i++){
    if (*(output+i)>*(output+max)){
      max = i;
    }
  }
  return 'A'+max;
}
