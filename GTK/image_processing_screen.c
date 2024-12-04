#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <gtk/gtk.h>

#include "../ImageProcessing/image_tools.h"
#include "image_processing_screen.h"
#include "solving_screen.h"
#include "toolchain_screen.h"

extern GtkWidget *window;
extern GtkWidget *box;
extern size_t version;

size_t max_version = 0;
SDL_Surface *surface = NULL;

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

void exec_filter(void filter(SDL_Surface *image)){
  filter(surface);
  version++;
  image_processing_screen();
}

void image_processing_screen(void) {
  char *image_path = NULL;
  asprintf(&image_path, "%s/image-%li.png", IMAGES_PATH, version);
  surface = IMG_Load(image_path);

  gtk_container_remove(GTK_CONTAINER(window), box);
  box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 50);
  GtkWidget *action_buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
  GtkWidget *filter_buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
  GtkWidget *plus_minus_buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
  GtkWidget *image = gtk_image_new_from_file(image_path);
  free(image_path);

  GtkWidget *undo = gtk_button_new_with_label("Grayscale");
  GtkWidget *redo = gtk_button_new_with_label("Black and White");
  GtkWidget *toolchain = gtk_button_new_with_label("Go back");
  GtkWidget *solve = gtk_button_new_with_label("Try to solve");
  g_signal_connect(undo, "clicked", G_CALLBACK(undo), NULL);
  g_signal_connect(redo, "clicked", G_CALLBACK(redo), NULL);
  g_signal_connect(toolchain, "clicked", G_CALLBACK(toolchain_screen), NULL);
  g_signal_connect(solve, "clicked", G_CALLBACK(solving_screen), NULL);
  gtk_box_pack_start(GTK_BOX(action_buttons), undo, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(action_buttons), redo, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(action_buttons), toolchain, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(action_buttons), solve, TRUE, TRUE, 10);

  GtkWidget *grayscale = gtk_button_new_with_label("Grayscale");
  GtkWidget *black_and_white_b = gtk_button_new_with_label("Black and White");
  GtkWidget *negatif_b = gtk_button_new_with_label("Negatif");
  GtkWidget *gauss_b = gtk_button_new_with_label("Gauss");
  g_signal_connect(grayscale, "clicked", G_CALLBACK(exec_filter), gray_level);
  g_signal_connect(black_and_white_b, "clicked", G_CALLBACK(exec_filter), black_and_white);
  g_signal_connect(negatif_b, "clicked", G_CALLBACK(exec_filter), negatif);
  g_signal_connect(gauss_b, "clicked", G_CALLBACK(exec_filter), gauss);
  gtk_box_pack_start(GTK_BOX(action_buttons), grayscale, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(action_buttons), black_and_white_b, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(action_buttons), negatif_b, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(action_buttons), gauss_b, TRUE, TRUE, 10);

  GtkWidget *increase_contrast = gtk_button_new_with_label("Contrast +");
  GtkWidget *decrease_contrast = gtk_button_new_with_label("Contrast -");
  GtkWidget *increase_luminosity = gtk_button_new_with_label("Luminosity +");
  GtkWidget *decrease_luminosity = gtk_button_new_with_label("Luminosity -");
  g_signal_connect(increase_contrast, "clicked", G_CALLBACK(exec_filter), get_contrast);
  g_signal_connect(decrease_contrast, "clicked", G_CALLBACK(exec_filter), get_contrast);
  g_signal_connect(increase_luminosity, "clicked", G_CALLBACK(exec_filter), get_contrast);
  g_signal_connect(decrease_luminosity, "clicked", G_CALLBACK(exec_filter), get_contrast);
  gtk_box_pack_start(GTK_BOX(action_buttons), undo, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(action_buttons), redo, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(action_buttons), toolchain, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(action_buttons), solve, TRUE, TRUE, 10);

  gtk_box_pack_start(GTK_BOX(box), action_buttons, FALSE, FALSE, 10);
  gtk_box_pack_start(GTK_BOX(box), image, FALSE, FALSE, 10);
  gtk_box_pack_start(GTK_BOX(box), filter_buttons, FALSE, FALSE, 10);
  gtk_box_pack_start(GTK_BOX(box), plus_minus_buttons, FALSE, FALSE, 10);
  gtk_container_add(GTK_CONTAINER(window), box);

  gtk_widget_show_all(window);
}
