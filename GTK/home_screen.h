#ifndef HOME_SCREEN_H
#define HOME_SCREEN_H

#include <gtk/gtk.h>

#define WORK_PATH "/tmp/OCR"
#define IMAGES_PATH "/tmp/OCR/Images"
#define LETTERS_PATH "/tmp/OCR/Letters"

#define LOGO_PATH "NVAM.ico"

void destroy_box(GtkWidget *box);

void home_screen(void);
void choose_file(void);

#endif
