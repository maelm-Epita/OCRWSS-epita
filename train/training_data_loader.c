#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_image.h>

float* image_to_input(char* path){
  SDL_Surface *img = IMG_Load(path);
  SDL_LockSurface(img);
  SDL_PixelFormat *format = img->format;
  Uint32* pixels = img->pixels;
  size_t size = img->w * img->h;
  float* input = calloc(size, sizeof(float));
  for (size_t i=0; i<size;i++){
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint32 pixel = *(pixels+i);
    SDL_GetRGB(pixel, format, &r, &g, &b);
    float greyscale = (float)(r+g+b)/3.0;
    *(input+i) = greyscale;
  }
  return input;
}
