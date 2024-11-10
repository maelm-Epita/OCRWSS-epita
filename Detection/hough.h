#ifndef HOUGH_H
#define HOUGH_H

#include <SDL2/SDL.h>

double** getBrightness(SDL_Surface* surface);

int** detectEdges(SDL_Surface* surface);

void detectLines(SDL_Surface *surface);

void drawLine(SDL_Surface* surface, double rho, double theta, int h, int w);

void drawLineOnSurface(SDL_Surface *surface, int x1, int y1, int x2, int y2);

void put_pixel(SDL_Surface *surface, int x, int y);

#endif
