#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>

#define LOGO_PATH "NVAM.ico"

void choose_file(void) {
  puts("zizi");
}

int main(int argc, char **argv) {
  // On déclare notre fenêtre window
  GtkWidget *window = NULL;

  // On demande à GTK de s'initialiser : il faut toujours faire passer argc et
  // argv en argument
  gtk_init(&argc, &argv);

  // On initialise notre window
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "NVAM - OCR");
  gtk_window_set_icon_from_file(GTK_WINDOW(window), LOGO_PATH, NULL);

  // Boîte verticale avec un espacement de 5 pixels
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  // Création d'un label
  GtkWidget *label =
      gtk_label_new("Bienvenue!\nSelectionnez un fichier pour commencer");

  GtkWidget *button = gtk_button_new_with_label("Selectionner un fichier");
  g_signal_connect(button, "clicked", G_CALLBACK(choose_file), NULL);

  gtk_box_pack_start(GTK_BOX(box), label, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(box), button, TRUE, TRUE, 0);
  gtk_container_add(GTK_CONTAINER(window), box);

  // On demande à GTK d'afficher notre window et tout ce qu'elle contient (rien
  // pour l'instant)
  gtk_widget_show_all(window);
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  gtk_container_set_border_width(GTK_CONTAINER(window), 10);

  // On demande à GTK de ne pas quitter et de laisser notre fenêtre ouverte
  gtk_main();
  return 0;
}
