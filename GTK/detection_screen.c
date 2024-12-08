#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <gtk/gtk.h>

#include "../Detection/detection.h"
#include "gtk_tools.h"
#include "screens.h"

extern GtkWidget *window;
extern GtkWidget *box;
extern cairo_surface_t *image_surface;
extern size_t version;

extern DrawingData DrawGrid;
extern DrawingData DrawWords;

int init = 0;

void draw_detection_lines(SDL_Surface *surface, DrawingData data) {
  for (int j = data.res[0]; j < data.res[2]; j++) {
    Uint8 *row = (Uint8 *)surface->pixels + j * surface->pitch;
    Uint32 *pixel =
        (Uint32 *)(row + data.res[1] * surface->format->BytesPerPixel);
    *pixel = SDL_MapRGB(surface->format, 255, 0, 0);
    pixel = (Uint32 *)(row + data.res[3] * surface->format->BytesPerPixel);
    *pixel = SDL_MapRGB(surface->format, 255 * data.colors[0],
                        255 * data.colors[0], 255 * data.colors[0]);
  }
}

void exec_detection() {
  char *cmd = NULL;
  asprintf(
      &cmd,
      "cp /tmp/OCR/Images/image-%li.bmp /tmp/OCR/Images/image-detection.bmp",
      version);
  system(cmd);
  free(cmd);
  SDL_Surface *surface = IMG_Load("/tmp/OCR/Images/image-detection.bmp");
  if (!init) {
    int res[8] = {0};
    detect_grid_and_word_list(surface, res);
    for (int i = 0; i < 4; i++) {
      DrawGrid.res[0] = res[0];
      DrawWords.res[0] = res[i + 4];
    }
    DrawGrid.res[0] = 98;
    DrawGrid.res[1] = 147;
    DrawGrid.res[2] = 735;
    DrawGrid.res[3] = 735;
    init = 1;
    printf("%i %i %i %i\n", DrawGrid.res[0], DrawGrid.res[1], DrawGrid.res[2],
           DrawGrid.res[3]);
  }
  //draw_detection_lines(surface, DrawGrid);
  //draw_detection_lines(surface, DrawWords);
  SDL_SaveBMP(surface, "/tmp/OCR/Images/image-detection.bmp");
  free(surface);
}

void detection_screen(void) {
  exec_detection();
  char *image_path = NULL;
  asprintf(&image_path, "%s/image-detection.bmp", IMAGES_PATH);

  gtk_container_remove(GTK_CONTAINER(window), box);
  box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
  GtkWidget *buttons_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);

  cairo_surface_destroy(image_surface);
  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(image_path, NULL);
  image_surface = create_cairo_surface_from_pixbuf(pixbuf);
  GtkWidget *drawing_area = gtk_drawing_area_new();
  g_object_unref(pixbuf);
  g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw), image_surface);

  free(image_path);

  GtkWidget *processing_button = gtk_button_new_with_label("Remodifiy Image");
  GtkWidget *other_file = gtk_button_new_with_label("Try another image");
  GtkWidget *solve_button = gtk_button_new_with_label("Try with detection");
  GtkWidget *frame_button = gtk_button_new_with_label("Set manually");

  g_signal_connect(processing_button, "clicked",
                   G_CALLBACK(image_processing_screen), NULL);
  g_signal_connect(other_file, "clicked", G_CALLBACK(choose_file), NULL);
  g_signal_connect(solve_button, "clicked", G_CALLBACK(solving_screen), NULL);
  g_signal_connect(frame_button, "clicked", G_CALLBACK(framing_screen), NULL);
  gtk_box_pack_start(GTK_BOX(buttons_box), processing_button, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(buttons_box), other_file, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(buttons_box), solve_button, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(buttons_box), frame_button, TRUE, TRUE, 10);

  gtk_box_pack_start(GTK_BOX(box), drawing_area, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(box), buttons_box, FALSE, FALSE, 10);
  gtk_container_add(GTK_CONTAINER(window), box);

  gtk_widget_show_all(window);
}
