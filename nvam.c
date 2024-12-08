#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WORK_PATH "/tmp/OCR"
#define IMAGES_PATH "/tmp/OCR/Images"
#define LETTERS_PATH "/tmp/OCR/Letters"

#include "GTK/screens.h"
#include "GTK/gtk_tools.h"

// On déclare notre fenêtre window
GtkWidget *window = NULL;
GtkWidget *box = NULL;
size_t version = 0;
cairo_surface_t *image_surface = NULL;

void quit() {
  gtk_widget_destroy(box);
  GList *children = gtk_container_get_children(GTK_CONTAINER(window));
  for (GList *child = children; child != NULL; child = child->next) {
    gtk_widget_destroy(GTK_WIDGET(child->data));
  }
  g_list_free(children);
  gtk_widget_destroy(window);
  gtk_main_quit();
}

int main(int argc, char **argv) {

  // On demande à GTK de s'initialiser : il faut toujours faire passer argc et
  // argv en argument
  gtk_init(&argc, &argv);

  //apply_theme("rose-pine-theme/gtk-3.0/gtk.css");
  apply_theme("GTK/Orchis-Pink/gtk-3.0/gtk.css");

  // On initialise notre window
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "NVAM - OCR");

  home_screen();

  gtk_widget_show_all(window);
  g_signal_connect(window, "destroy", G_CALLBACK(quit), NULL);
  gtk_container_set_border_width(GTK_CONTAINER(window), 10);

  // On demande à GTK de ne pas quitter et de laisser notre fenêtre ouverte
  gtk_main();
  return 0;
}
