#ifndef RESIZE_H
#define RESIZE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>


// Déclaration de la fonction pour récupérer la valeur d'un pixel en niveaux de gris
Uint8 get_gray(SDL_Surface *src, size_t x, size_t y);

// Déclaration de la fonction pour définir la valeur d'un pixel en niveaux de gris
void set_gray(SDL_Surface *dst, size_t x, size_t y, Uint8 val);

// Déclaration de la fonction d'interpolation cubique pour un segment
double cubic_inter(double pos1, double pos2, double pos3, double pos4, double x);

// Déclaration de la fonction d'interpolation bicubique pour une matrice 4x4
double bicubic_inter(double mat[4][4], double x, double y);

// Déclaration de la fonction de redimensionnement d'une image en utilisant l'interpolation bicubique
void resize(char* path);

#endif // RESIZE_H

