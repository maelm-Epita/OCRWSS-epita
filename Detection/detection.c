#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <sys/types.h>

#include "detection.h"

void lines(SDL_Surface *surface, block *block) {
  int w = surface->w;
  int h = surface->h;
  int x;
  Uint8 r;
  Uint32 *pixels = surface->pixels;

  int line = 0;  // mark the presence of line, here it has not
  int black = 0; // mark the presence of black p
  int begin;     // line of the beginning of black p's presence
  int end;       // line of end of it

  for (int y = block->min.y + 1; y < block->max.y + BLOCK_SIZE && y < h; y++) {
    for (x = block->min.x + 1; x < block->max.x + BLOCK_SIZE && x < w && !black;
         x++) {
      SDL_GetRGB(pixels[y * w + x], surface->format, &r, &r, &r);

      if (!r)
        black = 1;
    }

    if (black && !line) // if line has black ps
    {
      line = 1;
      begin = y; // conserve the index of the line
    }
    if (!black && line)
    // if previous line had black ps but now it is white
    {
      line = 0; // make back line = false
      end = y;  // conserve the index of the end

      double test = end - begin;
      double ratio = test / (double)w;

      if (ratio * 100 > 2) {
        int pos_begin = begin - 1; // previous line
        int pos_end = end + 1;     // next line
        for (x = block->min.x + 1; x < block->max.x + BLOCK_SIZE && x < w;
             x++) {
          if (pos_begin >= 0)
            pixels[pos_begin * w + x] =
                SDL_MapRGB(surface->format, 255, 105, 180);
          if (pos_end < h) // verify if next line can be reached
            pixels[pos_end * w + x] =
                SDL_MapRGB(surface->format, 255, 105, 180);
        }

        // trace the columns there because, if it has an end,
        // it has of course a beginning
        columns(surface, block, begin, end);
      }
    }

    black = 0; // make back black = false
  }
}

void columns(SDL_Surface *surface, block *block, int b_line, int e_line) {
  int w = surface->w;
  // variable for loops
  int x;
  int y;
  Uint8 r;
  Uint32 *pixels = surface->pixels;
  // it will necesserly have one black p one this line
  // and it marks the begin of encountering a caracter
  int end_c = 0;   // mark the end of an encounter with a caracter
  int black = 0;   // trace first line of the caracter
  int e_black = 0; // trace last line of the caracter
  int white = 0;   // if all this columns is white

  // loop from b_line to e_line
  // in wich there is another loop for all the columns

  for (x = block->min.x + 1; x < block->max.x + BLOCK_SIZE && x < w;
       x++) // through the columns
  {
    white = 1;

    for (y = b_line; y < e_line; y++) // through the lines
    {
      SDL_GetRGB(pixels[y * w + x], surface->format, &r, &r, &r);

      if (!r && !end_c) // first encounter with a cararcter
      {
        end_c = 1; // the begin of caracter is here
        black = 1; // must trace first line
      }
      if (!r) // if the caracter in not finished yet
        white = 0;
      // had encountered a character but now it's its end with y,
      // also verify if this column has not black p
      // with the white satement
      if (r == 255 && end_c && white && y == e_line - 1) {
        end_c = 0;   // the end of caracter is here
        e_black = 1; // must trace the last line
      }

      if (end_c && black) // color the previous column of the caracter
      {
        black = 0; // first line was traced, so make back the statement
        for (y = b_line; y < e_line; y++) {
          if (x - 1 > 0) // previous column
            // change color of column
            pixels[y * w + x - 1] = SDL_MapRGB(surface->format, 1, 149, 255);
        }
      }
      if (!end_c && e_black) // color the next column of the caracter
      {
        e_black = 0; // last line was traced, so make back the statement
        for (y = b_line; y < e_line; y++)
          // change color of column
          pixels[y * w + x] = SDL_MapRGB(surface->format, 1, 149, 255);
      }
    }
  }
}

SDL_Window *display_img(SDL_Surface *image) {
  if (SDL_VideoInit(NULL) < 0) // Initialize SDL
  {
    printf("Error of initializing SDL : %s", SDL_GetError());
  }

  // Create window
  SDL_Window *fenetre;

  int w = image->w; // width of the bmp
  int h = image->h; // height of the bmp

  fenetre =
      SDL_CreateWindow("OCR", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w,
                       h, SDL_WINDOW_RESIZABLE);

  if (fenetre == NULL) // if problem return error
  {
    printf("Error of creating window : %s", SDL_GetError());
  }

  SDL_BlitSurface(image, NULL, SDL_GetWindowSurface(fenetre), 0);
  SDL_UpdateWindowSurface(fenetre);

  return fenetre;
}

void wait_for_keypressed() {
  SDL_Event event;

  // Wait for a key to be down.
  do {
    SDL_PollEvent(&event);
  } while (event.type != SDL_KEYDOWN);

  // Wait for a key to be up.
  do {
    SDL_PollEvent(&event);
  } while (event.type != SDL_KEYUP);
}

int is_adjacent(block *block, int bx, int by) {
  return (bx >= block->min.x - BLOCK_SIZE && bx <= block->max.x + BLOCK_SIZE &&
          by >= block->min.y - BLOCK_SIZE && by <= block->max.y + BLOCK_SIZE);
}

block *get_block_adjacent(block **list_block, int nb_block, int bx, int by) {
  block *res = NULL;
  for (int i = 0; i < nb_block && res == NULL; i++) {
    if (list_block[i] != NULL && is_adjacent(list_block[i], bx, by))
      return list_block[i];
  }
  return res;
}

void check_merge(block **list_block, int nb_block) {
  for (int i = 0; i < nb_block; i++) {
    block *blocki = list_block[i];
    for (int j = i + 1; j < nb_block && blocki != NULL; j++) {
      block *blockj = list_block[j];
      if (blockj != NULL &&
          (is_adjacent(blockj, blocki->max.x, blocki->max.y) ||
           is_adjacent(blockj, blocki->min.x, blocki->min.y))) {
        blocki->max.x =
            blocki->max.x > blockj->max.x ? blocki->max.x : blockj->max.x;
        blocki->max.y =
            blocki->max.y > blockj->max.y ? blocki->max.y : blockj->max.y;
        blocki->min.x =
            blocki->min.x < blockj->min.x ? blocki->min.x : blockj->min.x;
        blocki->min.y =
            blocki->min.y < blockj->min.y ? blocki->min.y : blockj->min.y;
        // free(blockj);
        list_block[j] = NULL;
      }
    }
  }
}

void detect_grid_and_word_list(SDL_Surface *surface) {
  int w = surface->w;
  int h = surface->h;
  Uint32 *pixels = surface->pixels;

  block **list_block = NULL;
  int nb_block = 0;

  // browse by block
  // the image subdivided into blocks of block_size
  for (int by = 0; by < h; by += BLOCK_SIZE) {
    for (int bx = 0; bx < w; bx += BLOCK_SIZE) {
      // total number of black pixels in the block
      size_t nb_black_p = 0;
      // total number of pixels in the block
      int total = BLOCK_SIZE * BLOCK_SIZE;
      // get the numver of black pixels
      // we browse pixel by pixel through the block, making sure
      // not to go outside the image's bounds
      for (int y = by; y < by + BLOCK_SIZE && y < h; y++) {
        for (int x = bx; x < bx + BLOCK_SIZE && x < w; x++) {
          // we get a component of the pixel's color (does not matter because the image will be black and white)
          Uint8 r;
          SDL_GetRGB(pixels[y * w + x], surface->format, &r, &r, &r);
          // if this component is 0 then we know the pixel is black
          // if not it would be white (because the image is in black and white
          if (!r)
            nb_black_p++;
        }
      }
      // we get the density of the block
      double density = (double)nb_black_p / (double)total;
      if (density > THRESHOLD) {
        // we try to see if any block is adjacent
        block *blocka = get_block_adjacent(list_block, nb_block, bx, by);
        // if not we simply add the block to our list of blocks
        if (blocka == NULL) {
          nb_block++;
          list_block = realloc(list_block, nb_block * sizeof(struct block *));
          list_block[nb_block - 1] = malloc(sizeof(block));
          list_block[nb_block - 1]->min = (point){bx, by};
          list_block[nb_block - 1]->max =
              (point){bx + BLOCK_SIZE, by + BLOCK_SIZE};
        // otherwise we merge the two blocks
        } else {
          blocka->min.x = blocka->min.x < bx ? blocka->min.x : bx;
          blocka->min.y = blocka->min.y < by ? blocka->min.y : by;
          blocka->max.x = blocka->max.x > bx ? blocka->max.x : bx;
          blocka->max.y = blocka->max.y > by ? blocka->max.y : by;
        }
        // we check if these operations created any more merges to be done
        check_merge(list_block, nb_block);
      }
    }
  }
  // after finding all blocks, 
  for (int b = 0; b < nb_block; b++) {
    block *block = list_block[b];
    if (block != NULL) {
      for (int i = block->min.x; i < block->max.x + BLOCK_SIZE && i < w; i++) {
        pixels[block->min.y * w + i] = SDL_MapRGB(surface->format, 255, 0, 0);
        if (block->max.y + BLOCK_SIZE < h)
          pixels[(block->max.y + BLOCK_SIZE) * w + i] =
              SDL_MapRGB(surface->format, 255, 0, 0);
      }
      for (int i = block->min.y; i < block->max.y + BLOCK_SIZE && i < h; i++) {
        pixels[i * w + block->min.x] = SDL_MapRGB(surface->format, 255, 0, 0);
        if (block->max.x + BLOCK_SIZE < w)
          pixels[i * w + block->max.x + BLOCK_SIZE] =
              SDL_MapRGB(surface->format, 255, 0, 0);
      }

      lines(surface, block);
    }
  }

  for (int i = 0; i < nb_block; i++)
    free(list_block[i]);
  free(list_block);
}

double get_picture_density(SDL_Surface *surface) {
  int nb_black = 0;
  Uint32 *pixels = surface->pixels;
  Uint8 r;
  for (int i = 0; i < surface->w * surface->h; i++) {
    SDL_GetRGB(pixels[i], surface->format, &r, &r, &r);
    if (!r)
      nb_black++;
  }

  return (double)nb_black / (surface->w * surface->h);
}

int main(int argc, char *argv[]) {
  if (argc == 2) {
    SDL_Surface *image = IMG_Load(argv[1]);
    SDL_Window *screen = display_img(image);

    BLOCK_SIZE = image->w * image->h / 18000;
    THRESHOLD = get_picture_density(image) * 0.5;
    printf("BLOCK SIZE: %d   THRESHOLD: %f\n", BLOCK_SIZE, THRESHOLD);

    puts("init");

    SDL_BlitSurface(image, NULL, SDL_GetWindowSurface(screen), 0);
    puts("Blit");
    SDL_UpdateWindowSurface(screen);
    puts("Update");
    wait_for_keypressed();

    // lines(image);
    detect_grid_and_word_list(image);
    SDL_BlitSurface(image, NULL, SDL_GetWindowSurface(screen), 0);
    SDL_UpdateWindowSurface(screen);
    puts("END");
    wait_for_keypressed();

    SDL_FreeSurface(image);

    return 0;
  }

  return 1;
}
