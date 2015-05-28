#include <caca.h>
#include <curses.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>

#include "caca-driver.h"
#include "conchview-render.h"
#include "conchview.h"
#include "common-image.h"

#define CHARACTER_ASPECT_RATIO 2
#define STATUS_MAXLEN 64

static char const *startup_msgs[] = {
  "reconfiguring display...", "reticulating splines...", "contacting titans...",
  "approaching aural passages...", "monitoring plutonium phase changes...",
};

void conch_conchview_render(conchview *v, WINDOW *w, winrect *rect) {
  struct timeval now;
  struct image *i = v->imdata;

  if (i == NULL) {
    return;
  }
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

  int lines = rect->height;
  int cols = rect->width;

  float im_aspect = (float)i->h / (float)(i->w * CHARACTER_ASPECT_RATIO);

  int chk_lines = cols * im_aspect;
  if (chk_lines > lines) {
    cols = lines / im_aspect;
  } else {
    lines = chk_lines;
  }

  caca_canvas_t *cv = caca_create_canvas(rect->top, rect->left);
  caca_set_canvas_size(cv, cols, lines);
  caca_clear_canvas(cv);
  caca_set_dither_algorithm(i->dither, "none");
  caca_set_color_ansi(cv, CACA_DEFAULT, CACA_TRANSPARENT);
  caca_dither_bitmap(cv, 0, 0, cols, lines, i->dither, i->pixels);

  int start_x = (rect->width / 2) - (cols / 2);
  int start_y = (rect->height / 2) - (lines / 2);

  mvw_ncurses_display(w, rect->top + start_y, rect->left + start_x, cv);

  caca_free_canvas(cv);
}
