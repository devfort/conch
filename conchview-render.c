#include <caca.h>
#include <curses.h>

#include "caca-driver.h"
#include "conchview-render.h"
#include "conchview.h"
#include "common-image.h"

void conch_conchview_render(conchview *v, WINDOW *w, winrect *rect) {
  struct image *i = v->imdata;

  if (i == NULL) {
    return;
  }

  int lines = rect->height;
  int cols = rect->width;

  conch_status_set("Welcome to conch: reticulating splines...");

  caca_canvas_t *cv = caca_create_canvas(rect->top, rect->left);
  caca_add_dirty_rect(cv, 0, 0, cols, lines);
  caca_set_canvas_size(cv, cols, lines);
  caca_set_color_ansi(cv, CACA_DEFAULT, CACA_TRANSPARENT);
  caca_clear_canvas(cv);
  caca_set_dither_algorithm(i->dither, "none");
  caca_dither_bitmap(cv, 0, 0, cols, lines, i->dither, i->pixels);

  mvw_ncurses_display(w, rect->top, rect->left, cv);

  caca_free_canvas(cv);
}
