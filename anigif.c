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
  MagickExportImagePixels(wand, 0, 0, im_cols, im_rows, "BGRA", CharPixel,
                          pixels);

  caca_canvas_t *cv = caca_create_canvas(rect->top, rect->left);
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

  if (time_since_switch >= gif->delay) {
    if (MagickHasNextImage(gif->wand)) {
      MagickNextImage(gif->wand);
    } else {
      MagickResetIterator(gif->wand);
    }

    gif->last_switch_time = now;
    gif->delay = MagickGetImageDelay(gif->wand) || 1.0;
  }

  render_wand_to_screen(gif->wand, window, rect);
}

void anigif_free(anigif *gif) {
  gif->wand = DestroyMagickWand(gif->wand);
  free(gif);
}
