#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_opengl.h>

#include "../Solver/list_word.h"
#include "gtk_tools.h"

// Helper function to draw a filled circle
void draw_circle(SDL_Renderer *renderer, int cx, int cy, int radius) {
  for (int w = 0; w < radius * 2; w++) {
    for (int h = 0; h < radius * 2; h++) {
      int dx = radius - w; // Horizontal distance from center
      int dy = radius - h; // Vertical distance from center
      if ((dx * dx + dy * dy) <= (radius * radius)) {
        SDL_RenderDrawPoint(renderer, cx + dx, cy + dy);
      }
    }
  }
}

// Function to draw a rounded rectangle
void draw_rounded_rect(SDL_Renderer *renderer, int x, int y, int w, int h,
                       int radius) {
  // Draw the four corners as filled circles
  draw_circle(renderer, x + radius, y + radius, radius);     // Top-left corner
  draw_circle(renderer, x + w - radius, y + radius, radius); // Top-right corner
  draw_circle(renderer, x + radius, y + h - radius,
              radius); // Bottom-left corner
  draw_circle(renderer, x + w - radius, y + h - radius,
              radius); // Bottom-right corner

  // Draw the top, bottom, left, and right rectangles
  SDL_Rect rect_top = {x + radius, y, w - 2 * radius, radius};
  SDL_Rect rect_bottom = {x + radius, y + h - radius, w - 2 * radius, radius};
  SDL_Rect rect_left = {x, y + radius, radius, h - 2 * radius};
  SDL_Rect rect_right = {x + w - radius, y + radius, radius, h - 2 * radius};
  SDL_Rect rect_center = {x + radius, y + radius, w - 2 * radius,
                          h - 2 * radius};

  SDL_RenderFillRect(renderer, &rect_top);
  SDL_RenderFillRect(renderer, &rect_bottom);
  SDL_RenderFillRect(renderer, &rect_left);
  SDL_RenderFillRect(renderer, &rect_right);
  SDL_RenderFillRect(renderer, &rect_center);
}

// Function to draw a rounded rectangle between two letters
void draw(SDL_Renderer *renderer, letter letter1, letter letter2, int radius) {
  // Calculate the midpoints of the two letters
  int mid_x1 = (letter1.top_left.x + letter1.bot_right.x) / 2;
  int mid_y1 = (letter1.top_left.y + letter1.bot_right.y) / 2;
  int mid_x2 = (letter2.top_left.x + letter2.bot_right.x) / 2;
  int mid_y2 = (letter2.top_left.y + letter2.bot_right.y) / 2;

  // Calculate the rectangle's position and dimensions
  int rect_x = mid_x1 < mid_x2 ? mid_x1 : mid_x2;
  int rect_y = mid_y1 < mid_y2 ? mid_y1 : mid_y2;
  int rect_w = abs(mid_x2 - mid_x1);
  int rect_h = abs(mid_y2 - mid_y1);

  // Draw the rounded rectangle
  draw_rounded_rect(renderer, rect_x, rect_y, rect_w, rect_h, radius);
}

void draw_between_letters(const char *filepath, letter letter1,
                          letter letter2) {
  SDL_Surface *original_surface = IMG_Load(filepath);

  SDL_Window *window =
      SDL_CreateWindow("Dessiner un rectangle arrondi", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, original_surface->w,
                       original_surface->h, SDL_WINDOW_HIDDEN);
  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  // Créer une texture à partir de l'image chargée
  SDL_Texture *texture =
      SDL_CreateTextureFromSurface(renderer, original_surface);

  // Effacer le renderer et copier la texture source
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, NULL, NULL);

  // Dessiner le rectangle arrondi
  SDL_SetRenderDrawColor(renderer, 255, 0, 0,
                         SDL_ALPHA_OPAQUE); // Couleur rouge
  draw(renderer, letter1, letter2, 50);

  // Présenter le rendu
  SDL_RenderPresent(renderer);

  // Lire les pixels du rendu dans une surface
  SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(
      0, original_surface->w, original_surface->h, 32,
      SDL_PIXELFORMAT_ARGB8888);

  SDL_RenderReadPixels(renderer, NULL, surface->format->format, surface->pixels,
                       surface->pitch);

  // Sauvegarder l'image modifiée
  IMG_SavePNG(surface, "/tmp/OCR/Images/res.png");

  // Libérer les ressources
  SDL_FreeSurface(surface);
  SDL_FreeSurface(original_surface);
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
}

void draw_word(const char *filepath, const list_word *word) {
  SDL_Surface *original_surface = IMG_Load(filepath);

  SDL_Window *window = SDL_CreateWindow(
      "test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      original_surface->w, original_surface->h, SDL_WINDOW_HIDDEN);
  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  // Créer une texture à partir de l'image chargée
  SDL_Texture *texture =
      SDL_CreateTextureFromSurface(renderer, original_surface);

  // Effacer le renderer et copier la texture source
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, NULL, NULL);

  // Dessiner le rectangle arrondi
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 128);

  if (word->direction < 4) {
    int x = (word->start.top_left.x < word->end.top_left.x)
                ? word->start.top_left.x
                : word->end.top_left.x;
    int y = (word->start.top_left.y < word->end.top_left.y)
                ? word->start.top_left.y
                : word->end.top_left.y;

    int x_max = (word->start.bot_right.x > word->end.bot_right.x)
                    ? word->start.bot_right.x
                    : word->end.bot_right.x;
    int y_max = (word->start.bot_right.y > word->end.bot_right.y)
                    ? word->start.bot_right.y
                    : word->end.bot_right.y;

    int w = x_max - x;
    int h = y_max - y;
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(renderer, &rect);
    SDL_RenderPresent(renderer);
  }
  if (word->direction >= 4 && word->direction < 8) { // Diagonales uniquement
    // Points de départ et d'arrivée
    int x1 = word->start.top_left.x;
    int y1 = word->start.top_left.y;
    int x2 = word->end.bot_right.x;
    int y2 = word->end.bot_right.y;

    // Calcul des dimensions et vérification si diagonale
    int dx = x2 - x1;
    int dy = y2 - y1;

    // Calcul des coins du rectangle incliné
    int thickness = 5; // Épaisseur du rectangle
    int offset_x = (dy > 0) ? thickness : -thickness;
    int offset_y = (dx > 0) ? -thickness : thickness;

    // Coins du rectangle incliné
    int x3 = x2 + offset_x;
    int y3 = y2 + offset_y;
    int x4 = x1 + offset_x;
    int y4 = y1 + offset_y;

    // Activer le blending pour la transparence
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 128); // Rouge semi-transparent

    // Dessiner les côtés du rectangle (contours)
    SDL_RenderDrawLine(renderer, x1, y1, x2,
                       y2); // Ligne principale (diagonale)
    SDL_RenderDrawLine(renderer, x2, y2, x3, y3); // Côté 1
    SDL_RenderDrawLine(renderer, x3, y3, x4, y4); // Côté 2
    SDL_RenderDrawLine(renderer, x4, y4, x1, y1); // Côté 3

    // Remplir le rectangle en traçant des lignes horizontales
    int minX = (x1 < x2) ? x1 : x2;
    int maxX = (x3 > x4) ? x3 : x4;
    int minY = (y1 < y2) ? y1 : y2;
    int maxY = (y3 > y4) ? y3 : y4;

    // Remplir la zone entre les deux diagonales avec des lignes horizontales
    for (int y = minY; y <= maxY; y++) {
      int xStart = minX + (y - minY) * (x2 - minX) / (maxY - minY);
      int xEnd = minX + (y - minY) * (x3 - minX) / (maxY - minY);
      if (xStart > xEnd) {
        int temp = xStart;
        xStart = xEnd;
        xEnd = temp;
      }
      for (int x = xStart; x <= xEnd; x++) {
        SDL_RenderDrawPoint(renderer, x, y);
      }
    }

    SDL_RenderPresent(renderer);
  }

  // Lire les pixels du rendu dans une surface
  SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(
      0, original_surface->w, original_surface->h, 32,
      SDL_PIXELFORMAT_ARGB8888);

  SDL_RenderReadPixels(renderer, NULL, surface->format->format, surface->pixels,
                       surface->pitch);

  // SDL_Delay(5000);
  SDL_SaveBMP(surface, filepath);

  // Libérer les ressources
  SDL_FreeSurface(surface);
  SDL_FreeSurface(original_surface);
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
}
