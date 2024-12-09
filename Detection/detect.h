#pragma once

#include <SDL2/SDL_surface.h>

// a point is 2 coordinates
typedef struct{
  int x;
  int y;
} point;

// a shape is a collection of points
typedef struct{
  point* points;
  int pointnb;
} shape;

// a cell is a rectangle shape
typedef struct{
  point top_left;
  point bot_right;
} cell;


typedef struct{
  point top_left;
  point bot_right;
  char c;
} letter;


void detect(SDL_Surface *img, point grid_start, point grid_end, point list_start, point list_end, 
            letter** *grid_matrix, int* grid_mat_size_x, int* grid_mat_size_y,
            char** *word_list, int* word_list_size);
