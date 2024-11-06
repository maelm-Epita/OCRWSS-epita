#include <SDL2/SDL.h>
#include <math.h>

// Détection de contours simple (filtre de Sobel)
void sobel_filter(SDL_Surface *surface, Uint8 *edges) {
  int w = surface->w;
  int h = surface->h;
  Uint32 *pixels = surface->pixels;
  Uint8 r, g, b;

  for (int y = 1; y < h - 1; y++) {
    for (int x = 1; x < w - 1; x++) {
      int gx = 0, gy = 0;

      // Calcul des gradients de Sobel
      for (int ky = -1; ky <= 1; ky++) {
        for (int kx = -1; kx <= 1; kx++) {
          SDL_GetRGB(pixels[(y + ky) * w + (x + kx)], surface->format, &r, &g,
                     &b);
          int intensity =
              r; // Image en niveaux de gris, donc un seul canal suffit.

          // Matrices de Sobel pour x et y
          int sobel_x[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
          int sobel_y[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

          gx += intensity * sobel_x[ky + 1][kx + 1];
          gy += intensity * sobel_y[ky + 1][kx + 1];
        }
      }

      int magnitude = sqrt(gx * gx + gy * gy);
      edges[y * w + x] = (magnitude > 255) ? 255 : magnitude;
    }
  }
}

// Transformation de Hough pour la détection de lignes
void hough_transform(SDL_Surface *surface, Uint8 *edges, int threshold) {
  int w = surface->w;
  int h = surface->h;
  int max_rho = sqrt(w * w + h * h);
  int accumulator[2 * max_rho][180]; // Initialisation

  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      if (edges[y * w + x] >
          128) { // Seuil pour considérer un pixel comme un bord
        for (int theta = 0; theta < 180; theta++) {
          double radian = theta * M_PI / 180.0;
          int rho = (int)(x * cos(radian) + y * sin(radian));
          if (rho >= 0 && rho < 2 * max_rho) {
            accumulator[rho][theta]++;
          }
        }
      }
    }
  }

  // Marquer les lignes détectées sur l'image originale
  Uint32 *pixels = surface->pixels;
  for (int rho = 0; rho < 2 * max_rho; rho++) {
    for (int theta = 0; theta < 180; theta++) {
      if (accumulator[rho][theta] > threshold) {
        double radian = theta * M_PI / 180.0;
        for (int x = 0; x < w; x++) {
          int y = (int)((rho - x * cos(radian)) / sin(radian));
          if (y >= 0 && y < h) {
            pixels[y * w + x] =
                SDL_MapRGB(surface->format, 255, 0, 0); // Marquer en rouge
          }
        }
      }
    }
  }
}
