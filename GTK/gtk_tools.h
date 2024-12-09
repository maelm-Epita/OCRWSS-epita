#ifndef GTK_TOOLS_H
#define GTK_TOOLS_H

#include <SDL2/SDL_render.h>
#include <gtk/gtk.h>

#include "../Detection/detect.h"

typedef struct {
  gboolean is_drawing;
  gint start_x, start_y;
  gint end_x, end_y;
  int colors[3];
  int res[8];
} DrawingData;

cairo_surface_t *create_cairo_surface_from_pixbuf(const GdkPixbuf *pixbuf);
gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer data);

void apply_theme(const char *theme_path);

void free_all_elements_in_box(GtkBox *box);

void draw_between_letters(const char *filepath, letter letter1, letter letter2);
#endif
