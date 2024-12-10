#include <SDL2/SDL_image.h>
#include <gtk/gtk.h>

#include "../Detection/detect.h"
#include "../Solver/grid.h"
#include "../Solver/list_word.h"
#include "../Solver/multi_solver.h"
#include "gtk_tools.h"
#include "screens.h"

extern GtkWidget *window;
extern GtkWidget *box;
extern size_t version;
extern cairo_surface_t *image_surface;

extern DrawingData DrawGrid;
extern DrawingData DrawWords;

void solve(void) {
  char *cmd = NULL;
  asprintf(&cmd,
           "cp /tmp/OCR/Images/image-%li.bmp /tmp/OCR/Images/image-solve.bmp",
           version);
  system(cmd);
  free(cmd);
  char *image_path = "/tmp/OCR/Images/image-solve.bmp";
  printf("Grid start : %d, %d\nGrid end : %d, %d\n Word start : %d, %d\nWord "
         "end : %d, %d\n",
         DrawGrid.res[0], DrawGrid.res[1], DrawGrid.res[2], DrawGrid.res[3],
         DrawWords.res[0], DrawWords.res[1], DrawWords.res[2],
         DrawWords.res[3]);
  // Preparing arguments
  SDL_Surface *img = SDL_LoadBMP(image_path);
  point grid_start = {DrawGrid.res[0], DrawGrid.res[1]};
  point grid_end = {DrawGrid.res[2], DrawGrid.res[3]};
  point list_start = {DrawWords.res[0], DrawWords.res[1]};
  point list_end = {DrawWords.res[2], DrawWords.res[3]};
  // Preparing return variables
  int grid_size_x, grid_size_y, word_list_size;
  letter **grid_matrix;
  char **word_list;
  // detection
  detect(img, grid_start, grid_end, list_start, list_end, &grid_matrix,
         &grid_size_x, &grid_size_y, &word_list, &word_list_size);

  grid grid = {grid_size_x, grid_size_y, grid_matrix};
  list_word *res = NULL;
  res = find_all_words(&grid, word_list, word_list_size);

  while (res) {
    // printf("%s (%d, %d) -> (%d,%d)\n", res->word, res->start.x, res->start.y,
    // res->end.x, res->end.y);
    draw_word(image_path, res);
    res = res->next;
  }
}

void save_file() {
  GtkWidget *dialog;
  GtkFileChooser *chooser;
  GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
  gint res;

  dialog = gtk_file_chooser_dialog_new("Save File", GTK_WINDOW(window), action,
                                       ("_Cancel"), GTK_RESPONSE_CANCEL,
                                       ("_Save"), GTK_RESPONSE_ACCEPT, NULL);
  chooser = GTK_FILE_CHOOSER(dialog);

  gtk_file_chooser_set_do_overwrite_confirmation(chooser, TRUE);

  res = gtk_dialog_run(GTK_DIALOG(dialog));
  if (res == GTK_RESPONSE_ACCEPT) {
    char *filename;

    char *image_path = NULL;
    asprintf(&image_path, "%s/image-%li.bmp", IMAGES_PATH, version);
    // image_path = "/home/lepotototor/nvam.bmp";
    filename = gtk_file_chooser_get_filename(chooser);
    SDL_Surface *surface = IMG_Load(image_path);
    SDL_SaveBMP(surface, filename);
    SDL_FreeSurface(surface);
    // save_to_file(filename);
    g_free(filename);
    free(image_path);
  }

  gtk_widget_destroy(dialog);
}

void solving_screen(void) {
  solve();
  char *image_path = "/tmp/OCR/Images/image-solve.bmp";
  // image_path = "/home/lepotototor/nvam.bmp";

  gtk_container_remove(GTK_CONTAINER(window), box);
  box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
  GtkWidget *buttons_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);

  cairo_surface_destroy(image_surface);
  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(image_path, NULL);
  image_surface = create_cairo_surface_from_pixbuf(pixbuf);
  GtkWidget *drawing_area = gtk_drawing_area_new();
  g_object_unref(pixbuf);
  g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw), image_surface);

  GtkWidget *processing_button = gtk_button_new_with_label("Remodifiy Image");
  GtkWidget *save_button = gtk_button_new_with_label("Save the grid");
  GtkWidget *other_file = gtk_button_new_with_label("Try another image");
  GtkWidget *detection_button = gtk_button_new_with_label("View detection");

  g_signal_connect(processing_button, "clicked",
                   G_CALLBACK(image_processing_screen), NULL);
  g_signal_connect(save_button, "clicked", G_CALLBACK(save_file), NULL);
  g_signal_connect(other_file, "clicked", G_CALLBACK(choose_file), NULL);
  g_signal_connect(detection_button, "clicked", G_CALLBACK(detection_screen),
                   NULL);
  gtk_box_pack_start(GTK_BOX(buttons_box), processing_button, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(buttons_box), save_button, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(buttons_box), other_file, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(buttons_box), detection_button, TRUE, TRUE, 10);

  gtk_box_pack_start(GTK_BOX(box), drawing_area, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(box), buttons_box, FALSE, FALSE, 10);
  gtk_container_add(GTK_CONTAINER(window), box);

  gtk_widget_show_all(window);
}
