#include <cairo.h>
#include <gtk/gtk.h>

extern GtkWidget *window;
extern gboolean DrawGridRectangle;
extern gboolean DrawListRectangle;
extern gdouble grid_start_coord[2];
extern gdouble grid_end_coord[2];
extern gdouble list_start_coord[2];
extern gdouble list_end_coord[2];

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

  if (DrawGridRectangle) {
    int start_x = (grid_start_coord[0] - offset_x)/scale;
    int start_y = (grid_start_coord[1] - offset_y)/scale;
    int end_x = (grid_end_coord[0] - offset_x)/scale;
    int end_y = (grid_end_coord[1] - offset_y)/scale;
    double width = end_x - start_x;
    double height = end_y - start_y;
    cairo_set_line_width(cr, 2);
    cairo_set_source_rgb(cr, 1, 0, 0);  // Couleur rouge
    cairo_rectangle(cr, start_x, start_y, width, height);
    cairo_stroke(cr);  // Appliquer les dessins
  }
  if (DrawGridRectangle) {
    int start_x = (list_start_coord[0] - offset_x)/scale;
    int start_y = (list_start_coord[1] - offset_y)/scale;
    int end_x = (list_end_coord[0] - offset_x)/scale;
    int end_y = (list_end_coord[1] - offset_y)/scale;
    double width = end_x - start_x;
    double height = end_y - start_y;
    cairo_set_line_width(cr, 2);
    cairo_set_source_rgb(cr, 0, 0, 1);  // Couleur bleue
    cairo_rectangle(cr, start_x, start_y, width, height);
    cairo_stroke(cr);  // Appliquer les dessins
  }

  return FALSE;
}

void apply_theme(const char *theme_path) {
    GtkSettings *settings = gtk_settings_get_default();
    g_object_set(settings, "gtk-theme-name", "rose-pine-dawn", NULL);
    GtkCssProvider *provider = gtk_css_provider_new();
    GdkDisplay *display = gdk_display_get_default();
    GdkScreen *screen = gdk_display_get_default_screen(display);

    // Charge le fichier CSS
    if (!gtk_css_provider_load_from_path(provider, theme_path, NULL)) {
        g_printerr("Impossible de charger le fichier CSS : %s\n", theme_path);
        return;
    }

    // Applique le style au screen
    gtk_style_context_add_provider_for_screen(screen,
                                              GTK_STYLE_PROVIDER(provider),
                                              GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    g_object_unref(provider);
}

