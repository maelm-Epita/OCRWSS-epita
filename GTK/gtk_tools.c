#include <cairo.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <gtk/gtk.h>

extern GtkWidget *window;
extern gboolean DrawGridRectangle, DrawListRectangle;
extern int image_w, image_h;

double offset_x = 0, offset_y = 0, scale = 0;

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

  image_w = cairo_image_surface_get_width(surface);
  image_h = cairo_image_surface_get_height(surface);

  double scale_x = (double)window_width / image_w;
  double scale_y = (double)window_height / image_h;
  scale = scale_x < scale_y ? scale_x : scale_y;

  double scaled_width = image_w * scale;
  double scaled_height = image_h * scale;
  offset_x = (window_width - scaled_width) / 2;
  offset_y = (window_height - scaled_height) / 2;

  cairo_translate(cr, offset_x, offset_y);
  cairo_scale(cr, scale, scale);
  cairo_set_source_surface(cr, surface, 0, 0);
  cairo_paint(cr);

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
  gtk_style_context_add_provider_for_screen(
      screen, GTK_STYLE_PROVIDER(provider),
      GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

  g_object_unref(provider);
}


void free_all_elements_in_box(GtkBox *box) {
    GList *children, *iter;

    children = gtk_container_get_children(GTK_CONTAINER(box));
    for (iter = children; iter != NULL; iter = g_list_next(iter)) {
        GtkWidget *child = GTK_WIDGET(iter->data);
        gtk_container_remove(GTK_CONTAINER(box), child);
    }

    g_list_free(children);
}

