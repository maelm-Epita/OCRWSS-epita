#include <cairo.h>
#include <gtk/gtk.h>

extern GtkWidget *window;

cairo_surface_t *create_cairo_surface_from_pixbuf(const GdkPixbuf *pixbuf) {
  int width = gdk_pixbuf_get_width(pixbuf);
  int height = gdk_pixbuf_get_height(pixbuf);
  cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, width);

  cairo_surface_t *surface =
      cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
  cairo_t *cr = cairo_create(surface);

  gdk_cairo_set_source_pixbuf(cr, pixbuf, 0, 0);
  cairo_paint(cr);

  cairo_destroy(cr);
  return surface;
}

gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer data) {
  cairo_surface_t *surface = data;

  GtkAllocation allocation;
  gtk_widget_get_allocation(widget, &allocation);
  int window_width = allocation.width;
  int window_height = allocation.height;

  int image_width = cairo_image_surface_get_width(surface);
  int image_height = cairo_image_surface_get_height(surface);

  double scale_x = (double)window_width / image_width;
  double scale_y = (double)window_height / image_height;
  double scale = scale_x < scale_y ? scale_x : scale_y;

  double scaled_width = image_width * scale;
  double scaled_height = image_height * scale;
  double offset_x = (window_width - scaled_width) / 2;
  double offset_y = (window_height - scaled_height) / 2;

  cairo_translate(cr, offset_x, offset_y);
  cairo_scale(cr, scale, scale);
  cairo_set_source_surface(cr, surface, 0, 0);
  cairo_paint(cr);

  return FALSE;
}
