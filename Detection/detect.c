#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <sys/types.h>
#include <err.h>
#include "detect.h"

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


// when we come across a point p that is black, we want to find other points around it to form a complete shape
// this is a recursive function that finds the unvisited black points around a point and adds them to the shape
// it ends when there are no more unvisited black points around any point in the shape
void complete_shape(Uint32* pixels, SDL_PixelFormat *format, char** exhausted_points, int w, int h, shape* s, point p){
  // we will search for black points in a 3x3 with the point p at the center (all adjacent pixels)
  // starting from top left, to bottom right
  // we need to handle the case where the pixel is at the edge of the image 
  int start_y = max(p.y-1, 0);
  int start_x = max(p.x-1, 0);
  int end_y = max(p.y+1, h-1);
  int end_x = max(p.x+1, w-1);
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
          s->points = reallocarray(s->points, s->pointnb, sizeof(point));
          // err handling
          if (s->points == NULL){
            errx(EXIT_FAILURE, "Failed to realloc points of shape\n");
          }
          // append the new point the shape's point list
          s->points[s->pointnb-1] = bp;
          // we have to set the point as exhausted before the recursive call so that it is not added once again
          exhausted_points[y][x] = 1;
          complete_shape(pixels, format, exhausted_points, w, h, s, p);
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
  int min_x = 0;
  int min_y = 0;
  for (size_t i=0; i<sh.pointnb; i++){
    point p = sh.points[i];
    max_x = max(max_x, p.x);
    max_y = max(max_y, p.y);
    min_x = min(min_x, p.x);
    min_y = min(min_y, p.y);
  }
  cell res;
  res.top_left.x = min_x;
  res.top_left.y = min_y;
  res.bot_right.x = max_x;
  res.bot_right.y = max_y;
  return res;
}

cell* detect_grid(SDL_Surface *img, point grid_start, point grid_end){
  int w = img->w;
  int h = img->w;
  Uint32* pixels = img->pixels;
  SDL_PixelFormat* format = img->format;
  // we avoid going to points previously traversed with this list
  // we dynamically allocate the 2d matrix of pixels in the image
  char** exhausted_points = alloc_2d_matrix(h, w);
  // we will dynamically allocate the array of all cells found during the traversal
  int cell_nb = 0;
  cell* cells;
  // looping throught every pixel in the grid bounds, we will find all shapes and convert them to cells
  // while avoiding going to points multiple times
  for (int y=grid_start.y; y<=grid_end.y; y++){
    for (int x=grid_start.x; x<=grid_end.x; x++){
      // when we travel to a point, we first ignore if it is already exhausted
      if (exhausted_points[y][x]){
        continue;
      }
      // if it isnt, we set it to exhausted
      exhausted_points[y][x] = 1;
      Uint8 r;
      SDL_GetRGB(pixels[y * w + x], format, &r, &r, &r);
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
        complete_shape(pixels, format, exhausted_points, w, h, &sh, bp);
        // after completing the shape, we will convert it to a cell
        shape_to_cell(sh);
        // then we will reallocate the cell array and append our new cell
        cell_nb++;
        cells = reallocarray(cells, cell_nb-1, sizeof(cell));
      }
    }
  }
  return cells;
}

cell* detect_list(SDL_Surface *img, float* list_coord){
}

void detect(SDL_Surface *img, float* grid_coord, float* list_coord, letter** grid_matrix, char** word_list){
}

int main(int argc, char *argv[]) {
  if (argc == 2) {
    SDL_Surface *image = IMG_Load(argv[1]);
    SDL_Window *screen = display_img(image);

    float THRESHOLD = get_picture_density(image) * 0.5;
    puts("init");
    SDL_BlitSurface(image, NULL, SDL_GetWindowSurface(screen), 0);
    puts("Blit");
    SDL_UpdateWindowSurface(screen);
    puts("Update");
    wait_for_keypressed();

    detect();
    SDL_BlitSurface(image, NULL, SDL_GetWindowSurface(screen), 0);
    SDL_UpdateWindowSurface(screen);
    puts("END");
    wait_for_keypressed();

    SDL_FreeSurface(image);

    return 0;
  }

  return 1;
}
