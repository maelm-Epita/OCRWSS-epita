#include <gtk/gtk.h>

#include "screens.h"

extern GtkWidget *window;
extern GtkWidget *box;
extern size_t version;

void open_file(char *filename) {
  char *cmd = NULL;
  version = 0;
  asprintf(&cmd, "magick \"%s\" /tmp/OCR/Images/image-0.png", filename);
  system(cmd);
  free(cmd);

  toolchain_screen();
}

void choose_file(void) {
  GtkWidget *dialog = NULL;
  GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
  gint res;

  dialog = gtk_file_chooser_dialog_new("Open File", GTK_WINDOW(window), action,
                                       ("_Cancel"), GTK_RESPONSE_CANCEL,
                                       ("_Open"), GTK_RESPONSE_ACCEPT, NULL);

  res = gtk_dialog_run(GTK_DIALOG(dialog));
  if (res == GTK_RESPONSE_ACCEPT) {
    char *filename = NULL;
    GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(filter, "*.png");
    gtk_file_filter_add_pattern(filter, "*.bmp");
    gtk_file_filter_add_pattern(filter, "*.jpg");
    gtk_file_filter_add_pattern(filter, "*.jpeg");
    gtk_file_chooser_add_filter(chooser, filter);
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

  gtk_widget_show_all(window);
}
