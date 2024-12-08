#include "../ImageProcessing/image_tools.h"
#include "gtk_tools.h"
#include "screens.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <gtk/gtk.h>

extern GtkWidget *window;
extern GtkWidget *box;
extern size_t version;
extern cairo_surface_t *image_surface;
extern double offset_x, offset_y;
extern double scale;

int image_w = 0, image_h = 0;

DrawingData DrawGrid = {FALSE, 0, 0, 0, 0, {1, 0, 0}, {0}};
DrawingData DrawWords = {FALSE, 0, 0, 0, 0, {0, 0, 1}, {0}};
int current = 0;

void frame_grid(void) { current = 1; }

void frame_list(void) { current = 2; }

static void on_draw_event(__attribute__((unused)) GtkWidget *widget,
                          cairo_t *cr, gpointer user_data) {
  DrawingData *data = user_data;

  if (data->is_drawing ||
      (data->start_x != data->end_x && data->start_y != data->end_y)) {
    gint x = MIN(data->start_x, data->end_x);
    gint y = MIN(data->start_y, data->end_y);
    gint width = abs(data->end_x - data->start_x);
    gint height = abs(data->end_y - data->start_y);

    cairo_set_source_rgb(cr, data->colors[0], data->colors[1], data->colors[2]);
    cairo_rectangle(cr, x, y, width, height);
    cairo_stroke(cr);
  }
}

static gboolean on_button_press_event(__attribute__((unused)) GtkWidget *widget,
                                      GdkEventButton *event) {
  if (current == 0)
    return FALSE;

  DrawingData *data = current == 1 ? &DrawGrid : &DrawWords;

  if (event->button == GDK_BUTTON_PRIMARY) {
    data->start_x = event->x;
    data->start_y = event->y;
    data->end_x = event->x;
    data->end_y = event->y;
    data->is_drawing = TRUE;
  }

  return TRUE;
}

static gboolean on_button_release_event(GtkWidget *widget,
                                        GdkEventButton *event) {
  if (current == 0)
    return FALSE;

  DrawingData *data = current == 1 ? &DrawGrid : &DrawWords;
  if (data == &DrawWords)
    puts("oh no");

  if (event->button == GDK_BUTTON_PRIMARY &&
      data->is_drawing) { // Left mouse button
    data->end_x = event->x;
    data->end_y = event->y;
    data->is_drawing = FALSE;
    gtk_widget_queue_draw(widget); // Trigger a redraw
  }
  data->res[0] = (data->start_x - offset_x) / scale;
  data->res[1] = (data->start_y - offset_y) / scale;
  data->res[2] = (data->end_x - offset_x) / scale;
  data->res[3] = (data->end_y - offset_y) / scale;
  printf("(%d,%d)  -->  (%d,%d)\n", data->res[0], data->res[1], data->res[2],
         data->res[3]);
  current = 0;

  return TRUE;
}

static gboolean on_motion_notify_event(GtkWidget *widget,
                                       GdkEventMotion *event) {
  if (current == 0)
    return FALSE;

  DrawingData *data = current == 1 ? &DrawGrid : &DrawWords;

  if (data->is_drawing) {
    data->end_x = event->x;
    data->end_y = event->y;
    gtk_widget_queue_draw(widget); // Trigger a redraw
  }

  return TRUE;
}

void framing_screen(void) {
  // button reset
  gtk_container_remove(GTK_CONTAINER(window), box);
  box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
  GtkWidget *buttons_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);

  // image path setup
  char *image_path = NULL;
  asprintf(&image_path, "cp %s/image-%li.png %s/image-detection.png",
           IMAGES_PATH, version, IMAGES_PATH);
  system(image_path);
  free(image_path);
  asprintf(&image_path, "%s/image-detection.png", IMAGES_PATH);

  cairo_surface_destroy(image_surface);
  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(image_path, NULL);
  image_surface = create_cairo_surface_from_pixbuf(pixbuf);
  GtkWidget *drawing_area = gtk_drawing_area_new();
  g_object_unref(pixbuf);
  g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw), image_surface);
  free(image_path);

  // buttons
  GtkWidget *toolchain_button = gtk_button_new_with_label("Validate");
  GtkWidget *grid_button = gtk_button_new_with_label("Select Grid");
  GtkWidget *list_button = gtk_button_new_with_label("Select words's llist");
  // events
  gtk_widget_set_events(drawing_area, GDK_BUTTON_PRESS_MASK |
                                          GDK_BUTTON_RELEASE_MASK |
                                          GDK_POINTER_MOTION_MASK);

  g_signal_connect(toolchain_button, "clicked", G_CALLBACK(toolchain_screen),
                   NULL);
  g_signal_connect(grid_button, "clicked", G_CALLBACK(frame_grid), NULL);
  g_signal_connect(list_button, "clicked", G_CALLBACK(frame_list), NULL);

  g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw_event), &DrawWords);
  g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw_event), &DrawGrid);
  g_signal_connect(drawing_area, "button-press-event",
                   G_CALLBACK(on_button_press_event), NULL);
  g_signal_connect(drawing_area, "button-release-event",
                   G_CALLBACK(on_button_release_event), NULL);
  g_signal_connect(drawing_area, "motion-notify-event",
                   G_CALLBACK(on_motion_notify_event), NULL);

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
