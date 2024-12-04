#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOGO_PATH "NVAM.ico"

// On déclare notre fenêtre window
GtkWidget *window = NULL;
GtkWidget *box = NULL;
size_t version = 0;

void open_file(char *filename) {
  char *cmd = NULL;
  asprintf(&cmd, "magick \"%s\" /tmp/OCR/Images/image-0.png", filename);
  system(cmd);
  free(cmd);

  gtk_container_remove(GTK_CONTAINER(window), box);
  box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 50);

  GtkWidget *image = gtk_image_new_from_file(filename);
  gtk_box_pack_start(GTK_BOX(box), image, TRUE, TRUE, 0);
  gtk_container_add(GTK_CONTAINER(window), box);
  gtk_widget_show_all(window);
}

void choose_file(void) {
  GtkWidget *dialog;
  GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
  gint res;

  dialog = gtk_file_chooser_dialog_new("Open File", GTK_WINDOW(window), action,
                                       ("_Cancel"), GTK_RESPONSE_CANCEL,
                                       ("_Open"), GTK_RESPONSE_ACCEPT, NULL);

  res = gtk_dialog_run(GTK_DIALOG(dialog));
  if (res == GTK_RESPONSE_ACCEPT) {
    char *filename;
    GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
    filename = gtk_file_chooser_get_filename(chooser);
    open_file(filename);
    g_free(filename);
  }

  gtk_widget_destroy(dialog);
}

void home_screen() {
  system("rm -rf /tmp/OCR");
  system("mkdir -p /tmp/OCR/Images");
  system("mkdir -p /tmp/OCR/Letters");
  gtk_window_set_title(GTK_WINDOW(window), "NVAM - OCR");
  gtk_window_set_icon_from_file(GTK_WINDOW(window), LOGO_PATH, NULL);

  // Boîte verticale avec un espacement de 5 pixels
  box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
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
}

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
