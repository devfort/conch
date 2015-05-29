#include <assert.h>
#include <caca.h>
#include <sys/time.h>
#include <wand/MagickWand.h>

#include "anigif.h"
#include "caca-driver.h"
#include "render.h"

#define CHARACTER_ASPECT_RATIO 2

anigif *anigif_new(const char *filename) {
  anigif *gif = calloc(sizeof(anigif), 1);

  gif->wand = NewMagickWand();
  assert(gif->wand);

  MagickBooleanType ret = MagickReadImage(gif->wand, filename);
  assert(ret == MagickTrue);

  gif->delay = MagickGetImageDelay(gif->wand) * 10;

  gettimeofday(&gif->last_switch_time, NULL);
  return gif;
}

double difftimems(struct timeval *a, struct timeval *b) {
  // Compute the number of ms between the two timespecs
  double sec_diff = difftime(a->tv_sec, b->tv_sec);
  double usec_diff = a->tv_usec - b->tv_usec;
  return (sec_diff * 1000.0) + (usec_diff / 1000.0);
}

void render_wand_to_screen(MagickWand *wand, WINDOW *window, winrect *rect) {
  size_t im_cols = MagickGetImageWidth(wand);
  size_t im_rows = MagickGetImageHeight(wand);

  float lines = rect->height;
  float cols = rect->width;

  float im_aspect = (float)im_cols / (float)im_rows;
  float rect_aspect = (float)cols / (float)lines;

  if (im_aspect > rect_aspect) {
    lines = (cols / 2) * im_aspect;

    if (rect->height < lines) {
      lines = rect->height;
      cols = lines / im_aspect;
    }
  } else {
    cols = (lines * 2) * im_aspect;

    if (rect->width < cols) {
      cols = rect->width;
      lines = (cols / 2) / im_aspect;
    }
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
  MagickExportImagePixels(wand, 0, 0, im_cols, im_rows, "BGRA", CharPixel,
                          pixels);

  caca_canvas_t *cv = caca_create_canvas(0, 0);
  caca_set_canvas_size(cv, cols, lines);
  caca_clear_canvas(cv);
  caca_set_dither_algorithm(dither, "none");
  caca_dither_bitmap(cv, 0, 0, cols, lines, dither, pixels);

  int start_x = (rect->width / 2) - (cols / 2);
  int start_y = (rect->height / 2) - (lines / 2);

  mvw_ncurses_display(window, rect->top + start_y, rect->left + start_x, cv);

  caca_free_canvas(cv);
  free(pixels);
}

void anigif_render_frame(anigif *gif, WINDOW *window, winrect *rect) {
  struct timeval now;
  gettimeofday(&now, NULL);

  double time_since_switch = difftimems(&now, &gif->last_switch_time);
  while(time_since_switch >= gif->delay) {
    if (MagickHasNextImage(gif->wand)) {
      MagickNextImage(gif->wand);
    } else {
      MagickResetIterator(gif->wand);
      MagickNextImage(gif->wand);
    }

    time_since_switch -= gif->delay;
    gif->delay = MagickGetImageDelay(gif->wand) * 10;
  }

  gif->last_switch_time = now;

  render_wand_to_screen(gif->wand, window, rect);
}

void anigif_free(anigif *gif) {
  gif->wand = DestroyMagickWand(gif->wand);
  free(gif);
}
