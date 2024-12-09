#pragma once

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


void debgug(char* path, int grid_start_x, int grid_start_y, int grid_end_x, int grid_end_y,
            int list_start_x, int list_start_y, int list_end_x, int list_end_y);
