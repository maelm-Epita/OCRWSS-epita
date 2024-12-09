#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <gtk/gtk.h>

#include "../ImageProcessing/image_tools.h"
#include "gtk_tools.h"
#include "screens.h"

extern GtkWidget *window;
extern GtkWidget *box;
extern size_t version;
extern cairo_surface_t *image_surface;

size_t max_version = 0;

void undo(void) {
  if (version) {
    version--;
    image_processing_screen();
  }
}

void redo(void) {
  if (version < max_version) {
    version++;
    image_processing_screen();
  }
}

void exec_filter(__attribute__((unused)) GtkWidget *widget,
                 gpointer user_data) {
  void (*filter)(SDL_Surface *surface) = user_data;
  char *image_path = NULL;
  asprintf(&image_path, "%s/image-%li.bmp", IMAGES_PATH, version);
  SDL_Surface *surface = IMG_Load(image_path);
  filter(surface);
  version++;
  max_version = version > max_version ? version : max_version;
  free(image_path);
  asprintf(&image_path, "%s/image-%li.bmp", IMAGES_PATH, version);
  SDL_SaveBMP(surface, image_path);
  free(image_path);
  SDL_FreeSurface(surface);
  image_processing_screen();
}

void exec_rotate(){
  char *image_path = NULL;
  asprintf(&image_path, "%s/image-%li.bmp", IMAGES_PATH, version);
  SDL_Surface *surface = IMG_Load(image_path);
  surface = autoRotate(surface);
  version++;
  max_version = version > max_version ? version : max_version;
  free(image_path);
  asprintf(&image_path, "%s/image-%li.bmp", IMAGES_PATH, version);
  SDL_SaveBMP(surface, image_path);
  free(image_path);
  SDL_FreeSurface(surface);
  image_processing_screen();
}

void image_processing_screen(void) {
  char *image_path = NULL;
  asprintf(&image_path, "%s/image-%li.bmp", IMAGES_PATH, version);

  gtk_container_remove(GTK_CONTAINER(window), box);
  box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  GtkWidget *action_buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  GtkWidget *filter_buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  GtkWidget *plus_minus_buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

  cairo_surface_destroy(image_surface);
  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(image_path, NULL);
  image_surface = create_cairo_surface_from_pixbuf(pixbuf);
  GtkWidget *drawing_area = gtk_drawing_area_new();
  g_object_unref(pixbuf);
  g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw), image_surface);
  free(image_path);

  GtkWidget *undo_b = gtk_button_new_with_label("Undo");
  GtkWidget *redo_b = gtk_button_new_with_label("Redo");
  GtkWidget *toolchain = gtk_button_new_with_label("Validate");
  g_signal_connect(undo_b, "clicked", G_CALLBACK(undo), NULL);
  g_signal_connect(redo_b, "clicked", G_CALLBACK(redo), NULL);
  g_signal_connect(toolchain, "clicked", G_CALLBACK(toolchain_screen), NULL);
  gtk_box_pack_start(GTK_BOX(action_buttons), undo_b, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(action_buttons), redo_b, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(action_buttons), toolchain, TRUE, TRUE, 10);

  GtkWidget *grayscale = gtk_button_new_with_label("Grayscale");
  GtkWidget *black_and_white_b = gtk_button_new_with_label("Black and white");
  GtkWidget *negatif_b = gtk_button_new_with_label("Rotate");
  GtkWidget *gauss_b = gtk_button_new_with_label("Gauss");
  GtkWidget *median_b = gtk_button_new_with_label("Median");
  g_signal_connect(grayscale, "clicked", G_CALLBACK(exec_filter), gray_level);
  g_signal_connect(black_and_white_b, "clicked", G_CALLBACK(exec_filter),
                   black_and_white);
  g_signal_connect(negatif_b, "clicked", G_CALLBACK(exec_rotate), NULL);
  g_signal_connect(gauss_b, "clicked", G_CALLBACK(exec_filter), gauss);
  g_signal_connect(median_b, "clicked", G_CALLBACK(exec_filter), median);
  gtk_box_pack_start(GTK_BOX(filter_buttons), grayscale, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(filter_buttons), black_and_white_b, TRUE, TRUE,
                     10);
  gtk_box_pack_start(GTK_BOX(filter_buttons), negatif_b, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(filter_buttons), gauss_b, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(filter_buttons), median_b, TRUE, TRUE, 10);

  GtkWidget *inc_contrast = gtk_button_new_with_label("Contrast +");
  GtkWidget *dec_contrast = gtk_button_new_with_label("Contrast -");
  GtkWidget *inc_luminosity = gtk_button_new_with_label("Luminosity +");
  GtkWidget *dec_luminosity = gtk_button_new_with_label("Luminosity -");
  g_signal_connect(inc_contrast, "clicked", G_CALLBACK(exec_filter),
                   increase_contrast);
  g_signal_connect(dec_contrast, "clicked", G_CALLBACK(exec_filter),
                   decrease_contrast);
  g_signal_connect(inc_luminosity, "clicked", G_CALLBACK(exec_filter),
                   increase_brightness);
  g_signal_connect(dec_luminosity, "clicked", G_CALLBACK(exec_filter),
                   decrease_brightness);
  gtk_box_pack_start(GTK_BOX(plus_minus_buttons), inc_contrast, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(plus_minus_buttons), dec_contrast, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(plus_minus_buttons), inc_luminosity, TRUE, TRUE,
                     10);
  gtk_box_pack_start(GTK_BOX(plus_minus_buttons), dec_luminosity, TRUE, TRUE,
                     10);

  gtk_box_pack_start(GTK_BOX(box), action_buttons, FALSE, FALSE, 10);
  gtk_box_pack_start(GTK_BOX(box), drawing_area, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(box), filter_buttons, FALSE, FALSE, 10);
  gtk_box_pack_start(GTK_BOX(box), plus_minus_buttons, FALSE, FALSE, 10);
  gtk_container_add(GTK_CONTAINER(window), box);

  gtk_widget_show_all(window);
}
