#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <sys/types.h>

#include "detection.h"

double get_picture_density(const SDL_Surface *surface) {
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

int is_adjacent(block *block, int bx, int by, int BLOCK_SIZE) {
  return (bx >= block->min.x - BLOCK_SIZE && bx <= block->max.x + BLOCK_SIZE &&
          by >= block->min.y - BLOCK_SIZE && by <= block->max.y + BLOCK_SIZE);
}

block *get_block_adjacent(block **list_block, int nb_block, int bx, int by,
                          int BLOCK_SIZE) {
  block *res = NULL;
  for (int i = 0; i < nb_block && res == NULL; i++) {
    if (list_block[i] != NULL && is_adjacent(list_block[i], bx, by, BLOCK_SIZE))
      return list_block[i];
  }
  return res;
}

void check_merge(block **list_block, int nb_block, int BLOCK_SIZE) {
  for (int i = 0; i < nb_block; i++) {
    block *blocki = list_block[i];
    for (int j = i + 1; j < nb_block && blocki != NULL; j++) {
      block *blockj = list_block[j];
      if (blockj != NULL &&
          (is_adjacent(blockj, blocki->max.x, blocki->max.y, BLOCK_SIZE) ||
           is_adjacent(blockj, blocki->min.x, blocki->min.y, BLOCK_SIZE))) {
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

void detect_grid_and_word_list(const SDL_Surface *surface, int *res) {
  int BLOCK_SIZE = (double)surface->w * surface->h / 18000;
  double THRESHOLD = get_picture_density(surface);

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
          // we get a component of the pixel's color (does not matter because
          // the image will be black and white)
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
        block *blocka =
            get_block_adjacent(list_block, nb_block, bx, by, BLOCK_SIZE);
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
        check_merge(list_block, nb_block, BLOCK_SIZE);
      }
    }
  }

  if (nb_block > 0 && list_block[0]) {
    res[0] = list_block[0]->min.x;
    res[1] = list_block[0]->min.y;
    res[2] = list_block[0]->max.x + BLOCK_SIZE;
    res[3] = list_block[0]->max.y + BLOCK_SIZE;
  } else
    res[0] = res[1] = res[2] = res[3] = 0;
  if (nb_block > 1 && list_block[1]) {
    res[4] = list_block[1]->min.x;
    res[5] = list_block[1]->min.y;
    res[6] = list_block[1]->max.x + BLOCK_SIZE;
    res[7] = list_block[1]->max.y + BLOCK_SIZE;
  } else
    res[4] = res[5] = res[6] = res[7] = 0;

  for (int i = 0; i < nb_block; i++)
    free(list_block[i]);
  free(list_block);
}
