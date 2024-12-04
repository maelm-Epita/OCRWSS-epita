#include <gtk/gtk.h>

#include "home_screen.h"
#include "image_processing_screen.h"
#include "solving_screen.h"
#include "toolchain_screen.h"

extern GtkWidget *window;
extern GtkWidget *box;
extern size_t version;

void toolchain_screen() {
  char *image_path = NULL;
  asprintf(&image_path, "%s/image-%li.png", IMAGES_PATH, version);

  gtk_container_remove(GTK_CONTAINER(window), box);
  box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 50);
  GtkWidget *buttons_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
  GtkWidget *image = gtk_image_new_from_file(image_path);

  free(image_path);

  GtkWidget *processing_button = gtk_button_new_with_label("Modifier l'image");
  GtkWidget *solve_button = gtk_button_new_with_label("Resoudre la grille");
  GtkWidget *other_file =
      gtk_button_new_with_label("Selectionner un autre fichier");

  g_signal_connect(processing_button, "clicked",
                   G_CALLBACK(image_processing_screen), NULL);
  g_signal_connect(solve_button, "clicked", G_CALLBACK(solving_screen), NULL);
  g_signal_connect(other_file, "clicked", G_CALLBACK(choose_file), NULL);
  gtk_box_pack_start(GTK_BOX(buttons_box), processing_button, FALSE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(buttons_box), solve_button, FALSE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(buttons_box), other_file, FALSE, TRUE, 10);

  gtk_box_pack_start(GTK_BOX(box), image, FALSE, FALSE, 10);
  gtk_box_pack_start(GTK_BOX(box), buttons_box, TRUE, TRUE, 10);
  gtk_container_add(GTK_CONTAINER(window), box);

  gtk_widget_show_all(window);
}
