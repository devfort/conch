#include <caca.h>
#include <curses.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>

#include <wand/MagickWand.h>

#include "caca-driver.h"
#include "conchview-render.h"
#include "conchview.h"

#define CHARACTER_ASPECT_RATIO 2
#define STATUS_MAXLEN 64

static char const *startup_msgs[] = {
  "reconfiguring display...", "reticulating splines...", "contacting titans...",
  "approaching aural passages...", "monitoring plutonium phase changes...",
};

void conch_conchview_render(conchview *v, WINDOW *w, winrect *rect) {
  struct timeval now;

  gettimeofday(&now, NULL);

  if (!v->started) {
    conch_spinner_show();

    int n = 2 * (int)now.tv_sec + ((int)now.tv_usec / 5e5);
    size_t choice = n % (sizeof(startup_msgs) / sizeof(char const *));

    char *pre = "conch loading: ";
    int prelen = strlen(pre);

    char *msg = malloc(STATUS_MAXLEN * sizeof(char));
    strncpy(msg, pre, prelen);
    strncpy(msg + prelen, startup_msgs[choice], STATUS_MAXLEN - prelen);

    conch_status_set(msg);
    free(msg);
  }

  size_t im_cols = MagickGetImageWidth(v->wand);
  size_t im_rows = MagickGetImageHeight(v->wand);

  int lines = rect->height;
  int cols = rect->width;

  float im_aspect = (float)im_cols / (float)(im_rows * CHARACTER_ASPECT_RATIO);

  int chk_lines = cols * im_aspect;
  if (chk_lines > lines) {
    cols = lines / im_aspect;
  } else {
    lines = chk_lines;
  }

  int rmask = 0x00ff0000;
  int gmask = 0x0000ff00;
  int bmask = 0x000000ff;
  int amask = 0xff000000;
  int bpp = 32;
  int depth = 4;

  struct caca_dither *dither = caca_create_dither(
      bpp, im_cols, im_rows, depth * im_cols, rmask, gmask, bmask, amask);

  char *pixels = calloc(im_cols * im_rows, 4);
  MagickExportImagePixels(v->wand, 0, 0, im_cols, im_rows, "ARGB", CharPixel,
                          pixels);

  caca_canvas_t *cv = caca_create_canvas(rect->top, rect->left);
  caca_set_canvas_size(cv, cols, lines);
  caca_clear_canvas(cv);
  caca_set_dither_algorithm(dither, "none");
  caca_dither_bitmap(cv, 0, 0, cols, lines, dither, pixels);

  int start_x = (rect->width / 2) - (cols / 2);
  int start_y = (rect->height / 2) - (lines / 2);

  mvw_ncurses_display(w, rect->top + start_y, rect->left + start_x, cv);

  free(pixels);
  caca_free_canvas(cv);
}
