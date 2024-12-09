#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

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

void draw_between_letters(const char *filepath, letter letter1, letter letter2) {
  SDL_Surface *original_surface = IMG_Load(filepath);

  SDL_Window *window = SDL_CreateWindow(
      "Dessiner un rectangle arrondi", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      original_surface->w, original_surface->h, SDL_WINDOW_HIDDEN);
  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  // Créer une texture à partir de l'image chargée
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, original_surface);

  // Effacer le renderer et copier la texture source
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, NULL, NULL);

  // Dessiner le rectangle arrondi
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE); // Couleur rouge
  draw(renderer, letter1, letter2, 50);

  // Présenter le rendu
  SDL_RenderPresent(renderer);

  // Lire les pixels du rendu dans une surface
  SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(
      0, original_surface->w, original_surface->h, 32, SDL_PIXELFORMAT_ARGB8888);

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

