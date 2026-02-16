#include <SDL2/SDL_image.h>
#include <SDL2/SDL_surface.h>
#include <err.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>

void add_data(char* set_path, char* image_path, size_t label){
  // open training set file
  FILE *fptr = fopen(set_path, "a");
  // write label as first on line
  fprintf(fptr, "%lu,", label);
  // open image
  SDL_Surface *img = IMG_Load(image_path);
  SDL_LockSurface(img);
  SDL_PixelFormat *format = img->format;
  Uint32 *pixels = img->pixels;
  size_t size = img->w * img->h;
  // for each pixel
  for (size_t i = 0; i < size; i++) {
    Uint8 r, g, b;
    Uint32 pixel = pixels[i];
    SDL_GetRGB(pixel, format, &r, &g, &b);
    double greyscale = (double)(r + g + b) / 3.0;
    if (i < size-1){
      // write the greyscale followed by , if not the last
      fprintf(fptr, "%f,", greyscale);
    }
    else{
      // write the greyscale followed by nl if the last
      fprintf(fptr, "%f\n", greyscale);
    }
  }
  // close and free
  SDL_FreeSurface(img);
  fclose(fptr);
}

void add_dir(char* set_path, char* dir_path){
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window * window = SDL_CreateWindow("Image preview",
                                         SDL_WINDOWPOS_UNDEFINED, 
                                         SDL_WINDOWPOS_UNDEFINED,
                                         400, 400, 0);
  SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);
  struct dirent *dentry;
  DIR *dr = opendir(dir_path);
  if (dr == NULL){
    errx(EXIT_FAILURE, "Could not open dir\n");
  }
  while ((dentry = readdir(dr)) != NULL) {
    if (dentry->d_name[0] == '.'){
      continue;
    }
    printf("File : %s\n", dentry->d_name); 
    char* image_path;
    asprintf(&image_path, "%s/%s", dir_path, dentry->d_name);
    SDL_Surface * image = SDL_LoadBMP(image_path);
    SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, image);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
    printf("Enter Label : ");
    char label;
    scanf("%c%*c", &label);
    add_data(set_path, image_path, label-'A');
    free(image_path);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(image);
  }
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  closedir(dr);
  SDL_Quit();
}

int main(int argc, char** argv){
  if (argc == 4){
    if (strcmp(argv[1], "-d") == 0){
      add_dir(argv[2], argv[3]);
    }
    else{
      size_t label = **(argv+3)-'A';
      add_data(*(argv+1), *(argv+2), label);
    }
    return 0;
  }
  return 1;
}
