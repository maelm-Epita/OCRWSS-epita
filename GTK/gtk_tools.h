#ifndef GTK_TOOLS_H
#define GTK_TOOLS_H

#include <gtk/gtk.h>

cairo_surface_t *create_cairo_surface_from_pixbuf(const GdkPixbuf *pixbuf);
gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer data);

#endif
