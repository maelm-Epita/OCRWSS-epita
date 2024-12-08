#ifndef GTK_TOOLS_H
#define GTK_TOOLS_H

#include <gtk/gtk.h>

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

#endif
