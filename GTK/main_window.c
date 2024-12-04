#include <gtk/gtk.h>

#include <stdio.h>
#include <stdlib.h>
int main(int argc, char **argv) {
  // On déclare notre fenêtre window
  GtkWidget *window = NULL;
  // On demande à GTK de s'initialiser : il faut toujours faire passer argc et
  // argv en argument
  gtk_init(&argc, &argv);
  // On initialise notre window
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  // On demande à GTK d'afficher notre window et tout ce qu'elle contient (rien
  // pour l'instant)
  gtk_widget_show_all(window);
  // On demande à GTK de ne pas quitter et de laisser notre fenêtre ouverte
  gtk_main();
  return 0;
}
