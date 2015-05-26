#include <caca.h>
#include <curses.h>

#include "caca-driver.h"
#include "conchview-render.h"
#include "conchview.h"
#include "common-image.h"

// FIXME: remove these (duplicated) constants
#define BORDER_WIDTH 1
#define ORIGIN_X 0
#define ORIGIN_Y 0
#define PADDING_X 1
#define PADDING_Y 1

void render_conch(conchview *v, WINDOW *window) {
  struct image *i = v->imdata;

  if (i == NULL) {
    return;
  }

  int lines = getmaxy(window) - (2 * (BORDER_WIDTH + PADDING_Y));
  int cols = getmaxx(window) - (2 * (BORDER_WIDTH + PADDING_X));

  caca_canvas_t *cv = caca_create_canvas(ORIGIN_Y, ORIGIN_X);
  caca_add_dirty_rect(cv, ORIGIN_Y, ORIGIN_X, cols, lines);
  caca_set_canvas_size(cv, cols, lines);
  caca_set_color_ansi(cv, CACA_DEFAULT, CACA_TRANSPARENT);
  caca_clear_canvas(cv);
  caca_set_dither_algorithm(i->dither, "none");
  caca_dither_bitmap(cv, ORIGIN_Y, ORIGIN_X, cols, lines, i->dither, i->pixels);

  mvw_ncurses_display(window, (BORDER_WIDTH + PADDING_Y),
                      (BORDER_WIDTH + PADDING_X), cv);

  caca_free_canvas(cv);
}

void conch_conchview_render(conchview *v, WINDOW *w) {
  render_conch(v, w);
  wrefresh(w);
}
