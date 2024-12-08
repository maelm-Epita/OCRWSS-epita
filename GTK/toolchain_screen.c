#include <cairo.h>
#include <gtk/gtk.h>

#include "gtk_tools.h"
#include "screens.h"

extern GtkWidget *window;
extern GtkWidget *box;
extern size_t version;
extern cairo_surface_t *image_surface;

void toolchain_screen() {
  char *image_path = NULL;
  asprintf(&image_path, "%s/image-%li.bmp", IMAGES_PATH, version);

  gtk_container_remove(GTK_CONTAINER(window), box);
  box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
  GtkWidget *buttons_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

  if (image_surface != NULL)
    cairo_surface_destroy(image_surface);

  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(image_path, NULL);
  image_surface = create_cairo_surface_from_pixbuf(pixbuf);
  GtkWidget *drawing_area = gtk_drawing_area_new();
  g_object_unref(pixbuf);
  g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw), image_surface);

  free(image_path);

  GtkWidget *processing_button = gtk_button_new_with_label("Modify image");
  GtkWidget *detection_button =
      gtk_button_new_with_label("Start solving detection");
  GtkWidget *other_file = gtk_button_new_with_label("Choose another file");

  g_signal_connect(processing_button, "clicked",
                   G_CALLBACK(image_processing_screen), NULL);
  g_signal_connect(detection_button, "clicked", G_CALLBACK(detection_screen),
                   NULL);
  g_signal_connect(other_file, "clicked", G_CALLBACK(choose_file), NULL);
  gtk_box_pack_start(GTK_BOX(buttons_box), processing_button, TRUE, TRUE, 5);
  gtk_box_pack_start(GTK_BOX(buttons_box), detection_button, TRUE, TRUE, 5);
  gtk_box_pack_start(GTK_BOX(buttons_box), other_file, TRUE, TRUE, 5);

  gtk_box_pack_start(GTK_BOX(box), drawing_area, TRUE, TRUE, 5);
  gtk_box_pack_start(GTK_BOX(box), buttons_box, FALSE, FALSE, 5);
  gtk_container_add(GTK_CONTAINER(window), box);

  gtk_widget_show_all(window);
}
