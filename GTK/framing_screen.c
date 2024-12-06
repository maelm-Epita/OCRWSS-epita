#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <gtk/gtk.h>
#include "../ImageProcessing/image_tools.h"
#include "gtk_tools.h"
#include "screens.h"

extern GtkWidget *window;
extern GtkWidget *box;
extern size_t version;
gboolean DrawGridRectangle;
gboolean DrawListRectangle;
gdouble grid_start_coord[2];
gdouble grid_end_coord[2];
gdouble list_start_coord[2];
gdouble list_end_coord[2];
GtkWidget *indication_label;
gint click_coordinates[2];
int fg = 0;
int fl = 0;

gboolean drawing_rectangle(GtkWidget *widget, cairo_t *cr, gpointer data) {
  if (fg == 3){
    fg = 0;
  }
  if (fl == 3){
    fl = 0;
  }
  return FALSE;
}

void frame_grid(void){
  if (fg == 0){
    DrawGridRectangle = FALSE;
    fg = 1;
    fl = 0;
    gtk_label_set_text(GTK_LABEL(indication_label), "Encadrement de la grille : \nSelectionnez le premier coin.\n");
  }
  else if (fg == 2){
    g_print("First Coord : %d, %d\n", click_coordinates[0], click_coordinates[1]);
    grid_start_coord[0] = click_coordinates[0];
    grid_start_coord[1] = click_coordinates[1];
  }
  else if (fg == 3){
    g_print("Second Coord : %d, %d\n", click_coordinates[0], click_coordinates[1]);
    grid_end_coord[0] = click_coordinates[0];
    grid_end_coord[1] = click_coordinates[1];
    DrawGridRectangle = TRUE;
  }
}                        

void frame_list(void){
  if (fl == 0){
    DrawListRectangle = FALSE;
    fl = 1;
    fg = 0;
    gtk_label_set_text(GTK_LABEL(indication_label), "Encadrement de la grille : \nSelectionnez le premier coin.\n");
  }
  else if (fl == 2){
    g_print("First Coord : %d, %d\n", click_coordinates[0], click_coordinates[1]);
    list_start_coord[0] = click_coordinates[0];
    list_start_coord[1] = click_coordinates[1];
    gtk_label_set_text(GTK_LABEL(indication_label), "Encadrement de la grille : \nSelectionnez le second coin.\n");
  }
  else if (fl == 3){
    g_print("Second Coord : %d, %d\n", click_coordinates[0], click_coordinates[1]);
    list_end_coord[0] = click_coordinates[0];
    list_end_coord[1] = click_coordinates[1];
    DrawListRectangle = TRUE;
  }
}

gboolean drawing_clicked(GtkWidget *widget, GdkEventButton *event, gpointer data){
  if (event->button==1){
    click_coordinates[0] = event->x;
    click_coordinates[1] = event->y;
    g_print("Click Coord : %d, %d\n", click_coordinates[0], click_coordinates[1]);
    if (fg == 1){
      fg = 2;
      frame_grid();
    }
    else if (fg == 2){
      fg = 3;
      frame_grid();
      gtk_widget_queue_draw(widget);
      gtk_label_set_text(GTK_LABEL(indication_label), "");
    }
    else if (fl == 1){
      fl = 2;
      frame_list();
    }
    else if (fl == 2){
      fl = 3;
      frame_list();
      gtk_widget_queue_draw(widget);
      gtk_label_set_text(GTK_LABEL(indication_label), "");
    }
  }
  return FALSE;
}

void framing_screen(void) {
  // button reset
  gtk_container_remove(GTK_CONTAINER(window), box);
  box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
  GtkWidget *buttons_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);

  // image path setup
  char *image_path = NULL;
  asprintf(&image_path, "%s/image-%li.png", IMAGES_PATH, version);

  // load image
  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(image_path, NULL);
  cairo_surface_t *image_surface = create_cairo_surface_from_pixbuf(pixbuf);
  GtkWidget *drawing_area = gtk_drawing_area_new();
  g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw), image_surface);
  free(image_path);
  
  // Label
  indication_label =
    gtk_label_new("");
  // buttons
  GtkWidget *toolchain_button = gtk_button_new_with_label("Valider");
  GtkWidget *grid_button = gtk_button_new_with_label("Encadrer la grille");
  GtkWidget *list_button = gtk_button_new_with_label("Encadrer la liste de mots");
  // events
  gtk_widget_add_events(drawing_area, GDK_BUTTON_PRESS_MASK);
  g_signal_connect(toolchain_button, "clicked", G_CALLBACK(toolchain_screen), NULL);
  g_signal_connect(grid_button, "clicked", G_CALLBACK(frame_grid), NULL);
  g_signal_connect(list_button, "clicked", G_CALLBACK(frame_list), NULL);
  g_signal_connect(drawing_area, "button-press-event", G_CALLBACK(drawing_clicked), NULL);
  g_signal_connect(drawing_area, "draw", G_CALLBACK(drawing_rectangle), NULL);

  // label final setup
  gtk_box_pack_start(GTK_BOX(box), indication_label, FALSE, FALSE, 1);
  // buttons final setup
  gtk_box_pack_start(GTK_BOX(buttons_box), toolchain_button, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(buttons_box), grid_button, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(buttons_box), list_button, TRUE, TRUE, 10);
  // containers final setup
  gtk_container_add(GTK_CONTAINER(window), box);
  gtk_box_pack_start(GTK_BOX(box), drawing_area, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(box), buttons_box, FALSE, FALSE, 10);
  // show
  gtk_widget_show_all(window);
}
