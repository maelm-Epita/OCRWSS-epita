#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WORK_PATH "/tmp/OCR"
#define IMAGES_PATH "/tmp/OCR/Images"
#define LETTERS_PATH "/tmp/OCR/Letters"

#include "home_screen.h"

// On déclare notre fenêtre window
GtkWidget *window = NULL;
GtkWidget *box = NULL;
size_t version = 0;

int main(int argc, char **argv) {

  // On demande à GTK de s'initialiser : il faut toujours faire passer argc et
  // argv en argument
  gtk_init(&argc, &argv);

  // On initialise notre window
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  home_screen();

  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  gtk_container_set_border_width(GTK_CONTAINER(window), 10);

  // On demande à GTK de ne pas quitter et de laisser notre fenêtre ouverte
  gtk_main();
  return 0;
}
