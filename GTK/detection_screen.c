#include <gtk/gtk.h>

#include "gtk_tools.h"
#include "screens.h"

extern GtkWidget *window;
extern GtkWidget *box;
extern size_t version;

void detection_screen(void) {
  char *image_path = NULL;
  // asprintf(&image_path, "%s/image-solve.png", IMAGES_PATH);
  image_path = "/home/lepotototor/nvam.png";

  gtk_container_remove(GTK_CONTAINER(window), box);
  box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
  GtkWidget *buttons_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);

  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(image_path, NULL);
  cairo_surface_t *image_surface = create_cairo_surface_from_pixbuf(pixbuf);
  GtkWidget *drawing_area = gtk_drawing_area_new();
  g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw), image_surface);

  // free(image_path);

  GtkWidget *processing_button = gtk_button_new_with_label("Remodifiy Image");
  GtkWidget *other_file = gtk_button_new_with_label("Try another image");
  GtkWidget *solve_button = gtk_button_new_with_label("Go back to result");

  g_signal_connect(processing_button, "clicked",
                   G_CALLBACK(image_processing_screen), NULL);
  g_signal_connect(other_file, "clicked", G_CALLBACK(choose_file), NULL);
  g_signal_connect(solve_button, "clicked", G_CALLBACK(solving_screen), NULL);
  gtk_box_pack_start(GTK_BOX(buttons_box), processing_button, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(buttons_box), other_file, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(buttons_box), solve_button, TRUE, TRUE, 10);

  gtk_box_pack_start(GTK_BOX(box), drawing_area, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(box), buttons_box, FALSE, FALSE, 10);
  gtk_container_add(GTK_CONTAINER(window), box);

  gtk_widget_show_all(window);
}
