#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <err.h>
#include "detect.h"
#include "../DeepLearning/neural-net/Network.h"

#define NETWORK_PATH "../DeepLearning/models/easy-final.model"

int MAX_CELL_AREA;
int MIN_CELL_AREA;
int PADDING;
double CELL_LENGTH_MAX_ERR_FACTOR;

int min(int n1, int n2){
  if (n1 < n2){
    return n1;
  }
  return n2;
}
int max(int n1, int n2){
  if (n1 > n2){
    return n1;
  }
  return n2;
}

// helper function to allocate the 2d matrix of pixels in the image to track exhausted points
char** alloc_2d_matrix(int h, int w){
  char** mat = calloc(h, sizeof(char*));
  for (int i=0; i<h; i++){
    mat[i] = calloc(w, sizeof(char));
  }
  return mat;
}

void free_2d_matrix(char** mat, int h){
  for (int i=0; i<h; i++){
    free(mat[i]);
  }
  free(mat);
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

void draw_vertical_line(SDL_Surface *surface, int y1, int y2, int x){
  Uint32* pixels = surface->pixels;
  int w = surface->w;
  int begin = min(y1, y2);
  int end = max(y1, y2);
  for (int y=begin; y<=end; y++){
    pixels[y*w + x] = SDL_MapRGB(surface->format, 255,0,0);
  }
}

void draw_horizontal_line(SDL_Surface *surface, int x1, int x2, int y){
  Uint32* pixels = surface->pixels;
  int w = surface->w;
  int begin = min(x1, x2);
  int end = max(x1, x2);
  for (int x=begin; x<=end; x++){
    pixels[y*w + x] = SDL_MapRGB(surface->format, 255,0,0);
  }
}

void draw_rectangle(SDL_Surface *surface, point p1, point p2){
  // Draw horizontal line between p1.x and p2.x at the shared p1.y value
  draw_horizontal_line(surface, p1.x, p2.x, p1.y);
  // Draw horizontal line between p1.x and p2.x at the shared p2.y value
  draw_horizontal_line(surface, p1.x, p2.x, p2.y);
  // Draw vertical line between p1.y and p2.y at the shared p1.x value
  draw_vertical_line(surface, p1.y, p2.y, p1.x);
  // Draw vertical line between p1.y and p2.y at the shared p2.x value
  draw_vertical_line(surface, p1.y, p2.y, p2.x);
}

int cell_area(cell c){
  int w = c.bot_right.x-c.top_left.x;
  int h = c.bot_right.y-c.top_left.y;
  return h*w;
}

// when we come across a point p that is black, we want to find other points around it to form a complete shape
// this is a recursive function that finds the unvisited black points around a point and adds them to the shape
// it ends when there are no more unvisited black points around any point in the shape
void complete_shape(Uint32* pixels, SDL_PixelFormat *format, int pitch, char** exhausted_points, int w, int h, shape* s, point p){
  // we will search for black points in a 3x3 with the point p at the center (all adjacent pixels)
  // starting from top left, to bottom right
  // we need to handle the case where the pixel is at the edge of the image 
  int start_y = max(p.y-1, 0);
  int start_x = max(p.x-1, 0);
  int end_y = min(p.y+1, h-1);
  int end_x = min(p.x+1, w-1);
  for (int y = start_y; y<=end_y; y++){
    for (int x = start_x; x<=end_x; x++){
      // if we find a black pixel that has not been exhausted and is not the point at the center (which is already in the shape)
      // we add it to the shape
      // and then we launch the algorithm at this pixel recursively
      // since the image is black and white, if any component of the pixel's colors is positive, that means it is white
      // we start by checking if the point has been exhausted or is the center point,
      // this is to avoid unnecessary calculations
      // we will add the point to the list of exhausted points regardless of if it belongs to the shape
      if (!exhausted_points[y][x] && x!=p.x && y!=p.y){
        Uint8 r;
        SDL_GetRGB(pixels[y * w + x], format, &r, &r, &r);
        if (!r){
          point bp = {x,y};
          s->pointnb++;
          s->points = realloc(s->points, s->pointnb * sizeof(point));
          // err handling
          if (s->points == NULL){
            errx(EXIT_FAILURE, "Failed to realloc points of shape\n");
          }
          // append the new point the shape's point list
          s->points[s->pointnb-1] = bp;
          // we have to set the point as exhausted before the recursive call so that it is not added once again
          exhausted_points[y][x] = 1;
          complete_shape(pixels, format, pitch, exhausted_points, w, h, s, bp);
        }
      }
      // the point might be set to exhausted twice but this does not atter
      exhausted_points[y][x] = 1;
    }
  }
}

// Converts a shape to it's bounding cell
cell shape_to_cell(shape sh){
  // Get the shape's max x coordinate
  // Get the shape's max y coordinate
  // Get the shape's min x coordinate
  // Get the shape's min y coordinate
  // Create the cell from the two points min (top left) and max (bot right)
  int max_x = 0;
  int max_y = 0;
  int min_x = -1;
  int min_y = -1;
  for (int i=0; i<sh.pointnb; i++){
    point p = sh.points[i];
    printf("Shape: Point = (%d, %d)\n", p.x, p.y);
    if (min_x == -1){
      min_x = p.x;
    }
    else{
      min_x = min(min_x, p.x);
    }
    if (min_y == -1){
      min_y = p.y;
    }
    else{
      min_y = min(min_y, p.y);
    }
    max_x = max(max_x, p.x);
    max_y = max(max_y, p.y);
  }
  cell res;
  res.top_left.x = min_x;
  res.top_left.y = min_y;
  res.bot_right.x = max_x;
  res.bot_right.y = max_y;
  return res;
}

//
//
// TODO : REMOVE THE MAKE SQUARE AND PAD FROM THE FILTERING AS THEY WILL INTRODUCE OVERLAP
// INSTEAD PAD AND CORRECT ASPECT RATIO AFTER SAVING THE IMAGE FILE
// MAKE A FUNCTION TO SPLIT UP THE SHAPES IF THEY ARE ONLY HELD BY FEW PIXELS
//
//
// This function splits cells that contain multiple letters
// it checks the length of the cell and compares it to an average length
// if it is too big, we split it into multiple cells of the expected size
int separate_cells(cell* cells, int cell_nb, cell** new_cells_p){
  if (*new_cells_p != NULL){
    free(*new_cells_p);
  }
  cell* new_cells = NULL;
  int new_cell_nb = 0;
  // first we get the average ratio of width/height
  double expected_ratio = 0;
  for (int i=0; i<cell_nb; i++){
    cell c = cells[i];
    double width = c.bot_right.x - c.top_left.x;
    double height = c.bot_right.y - c.top_left.y;
    double ratio = width/height;
    expected_ratio+=ratio;
  }
  expected_ratio/=cell_nb;
  printf("Expected ratio: %f\n", expected_ratio);
  // Now we compare and possibly split each cell
  for (int i=0; i<cell_nb; i++){
    cell c = cells[i];
    int width = c.bot_right.x - c.top_left.x;
    int height = c.bot_right.y - c.top_left.y;
    int ratio = width/height;
    // if the length is alot bigger (at least 2x bigger) than the expected length)
    if (ratio >= expected_ratio * CELL_LENGTH_MAX_ERR_FACTOR){
      // split
      // calculate the expected width
      int expected_width = height*expected_ratio;
      // find the amount of cells to split into
      int split_nb = width/expected_width;
      // if we would only split into once cell, we can just add the cell as it was and skip this iteration
      if (split_nb == 1){
        new_cell_nb++;
        new_cells = realloc(new_cells, new_cell_nb*sizeof(cell));
        new_cells[new_cell_nb-1] = c;
        continue;
      }
      // find the spacing between each cell
      int pad_nb = width/split_nb - expected_width;
      // each new cell's top left point x will be this much off of the last
      int offset = pad_nb + expected_width;
      for (int j=0; j<split_nb; j++){
        point tl = {c.top_left.x + offset*j, c.top_left.y};
        point br = {tl.x + expected_width, c.bot_right.y};
        cell c = {tl, br};
        new_cell_nb++;
        new_cells = realloc(new_cells, new_cell_nb*sizeof(cell));
        new_cells[new_cell_nb-1] = c;
      }
    }
    // otherwise just add it to the cells
    else{
      new_cell_nb++;
      new_cells = realloc(new_cells, new_cell_nb*sizeof(cell));
      new_cells[new_cell_nb-1] = c;
    }
  }
  free(cells);
  *new_cells_p = new_cells;
  return new_cell_nb;
}

// Makes the cell into a square which's sides lengths are equal
// to the greatest side length of the original rectangle
// keeps the center the same
void make_square(cell* cell){
  int w = cell->bot_right.x-cell->top_left.x;
  int h = cell->bot_right.y-cell->top_left.y;
  if (w>h){
    int diff = w-h;
    int pad = diff/2;
    cell->top_left.y-=pad;
    cell->bot_right.y+=pad;
  }
  else if (h>w){
    int diff = h-w;
    int pad = diff/2;
    cell->top_left.x-=pad;
    cell->bot_right.x+=pad;
  }
}

// Adds padding to the cell while keeping the center the same
void pad(cell *cell){
  // Padding horizontal
  cell->top_left.x-=PADDING;
  cell->bot_right.x+=PADDING;
  // Padding vertical
  cell->top_left.y-=PADDING;
  cell->bot_right.y+=PADDING;
}

int filter_and_refine_cells(cell* cells, int cell_nb, cell** new_cells_p){
  // we check if the array was not null so we dont leak any memory
  if (*new_cells_p != NULL){
    free(*new_cells_p);
  }
  cell* new_cells = NULL;
  int new_cell_nb = 0;
  // we loop through every cell, deciding to append it to the new cell array 
  // based on filters
  for (int i = 0; i<cell_nb; i++){
    cell cp = cells[i];
    cell c = cells[i];
    make_square(&c);
    pad(&c);
    int area = cell_area(c);
    if (area <= MAX_CELL_AREA && area >= MIN_CELL_AREA){
      new_cell_nb++;
      new_cells = realloc(new_cells, new_cell_nb*sizeof(cell));
      new_cells[new_cell_nb-1] = cp;
    }
  }
  free(cells);
  *new_cells_p = new_cells;
  return new_cell_nb;
}

int detect_cells(SDL_Surface *img, point area_start, point area_end, cell** cell_list){
  // free any cell list that was provided
  if (*cell_list != NULL){
    free(*cell_list);
  }
  int w = img->w;
  int h = img->h;
  Uint32* pixels = img->pixels;
  SDL_PixelFormat* format = img->format;
  // we avoid going to points previously traversed with this list
  // we dynamically allocate the 2d matrix of pixels in the image
  char** exhausted_points = alloc_2d_matrix(h, w);
  // we will dynamically allocate the array of all cells found during the traversal
  int cell_nb = 0;
  cell* cells = NULL;
  // looping throught every pixel in the grid bounds, we will find all shapes and convert them to cells
  // while avoiding going to points multiple times
  for (int y=area_start.y; y<=area_end.y; y++){
    for (int x=area_start.x; x<=area_end.x; x++){
      // when we travel to a point, we first ignore if it is already exhausted
      if (exhausted_points[y][x]){
        continue;
      }
      // if it isnt, we set it to exhausted
      exhausted_points[y][x] = 1;
      Uint8 r;
      SDL_GetRGB(pixels[y * img->w + x], format, &r, &r, &r);
      //pixels[y*img->w + x] = SDL_MapRGB(img->format, 255,0,0);
      //SDL_BlitSurface(img, NULL, SDL_GetWindowSurface(screen), 0);
      //SDL_UpdateWindowSurface(screen);
      // if it is black, we have found a shape
      // we create the shape and complete it from the point
      if (!r){
        point bp = {x,y};
        shape sh;
        sh.pointnb = 1;
        sh.points = calloc(1, sizeof(point));
        // err handling
        if (sh.points == NULL){
          errx(EXIT_FAILURE, "Failed to alloc points of shape\n");
        }
        sh.points[0] = bp;
        complete_shape(pixels, format, img->pitch, exhausted_points, w, h, &sh, bp);
        // after completing the shape, we will convert it to a cell
        // then we will reallocate the cell array and append our new cell
        cell_nb++;
        cells = realloc(cells, cell_nb * sizeof(cell));
        // err handling
        if (cells == NULL){
          errx(EXIT_FAILURE, "Could not reallocate cell array\n");
        }
        cells[cell_nb-1] = shape_to_cell(sh);
        // finally we free the shape's points
        free(sh.points);
      }
    }
  }
  free_2d_matrix(exhausted_points, h);
  // after we are done we will filter cells to keep only likely letters
  // we also apply transformations such as correcting the aspect ratio
  // to be a square while keeping the letter centered
  cell* separated_cells = NULL;
  cell_nb = separate_cells(cells, cell_nb, &separated_cells);
  cell* filtered_cells = NULL;
  cell_nb = filter_and_refine_cells(separated_cells, cell_nb, &filtered_cells);
  *cell_list = filtered_cells;
  return cell_nb;
}

void detect(SDL_Surface *img, point grid_start, point grid_end, point list_start, point list_end, letter** grid_matrix, char** word_list){
  cell* grid_cells = NULL;
  cell* list_cells = NULL;
  int grid_cell_nb = detect_cells(img, grid_start, grid_end, &grid_cells);
  int list_cell_nb = detect_cells(img, list_start, list_end, &list_cells);
  printf("gnb : %d, lnb : %d\n", grid_cell_nb, list_cell_nb);
  if (grid_cells == NULL){
    errx(EXIT_FAILURE, "grid is null \n");
  }
  for (int i = 0; i<grid_cell_nb; i++){
    cell c = grid_cells[i];
    draw_rectangle(img, c.top_left, c.bot_right);
  }
  for (int i = 0; i<list_cell_nb; i++){
    cell c = list_cells[i];
    draw_rectangle(img, c.top_left, c.bot_right);
  }
  // Convert results to input of solver
  // Make 2d grid cell matrix
  // Add cells according to coordinates
  // Make 2d word list matrix
  // Add cells according to coordinates
  // Load AI
  //printf("Loading AI model...\n");
  //struct Network net = load_network(NETWORK_PATH);
  // For each cell, save it and call the network on it 
  // Then add it to the corresponding place in the grid matrix or word list

  if (grid_cells != NULL){
    free(grid_cells);
  }
  if (list_cells != NULL){
    free(list_cells);
  }
}

int main(int argc, char *argv[]) {
  if (argc == 2) {
    SDL_Surface *image = IMG_Load(argv[1]);
    SDL_Window *screen = display_img(image);
    // magic constants
    MAX_CELL_AREA = (image->w*image->h)/300;
    MIN_CELL_AREA = (image->w*image->h)/3000;
    PADDING = 5;
    CELL_LENGTH_MAX_ERR_FACTOR = 1;
    // program
    puts("init");
    SDL_BlitSurface(image, NULL, SDL_GetWindowSurface(screen), 0);
    puts("Blit");
    SDL_UpdateWindowSurface(screen);
    puts("Update");
    wait_for_keypressed();
    point start = {0,0};
    point end = {image->w-1,image->h-1};
    detect(image, start, end, start, end, NULL, NULL);
    SDL_BlitSurface(image, NULL, SDL_GetWindowSurface(screen), 0);
    SDL_UpdateWindowSurface(screen);
    puts("END");
    wait_for_keypressed();
    wait_for_keypressed();

    SDL_FreeSurface(image);

    return 0;
  }

  return 1;
}
