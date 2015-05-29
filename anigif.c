#include <assert.h>
#include <wand/MagickWand.h>

#include "anigif.h"

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

void anigif_free(anigif *gif) {
  gif->wand = DestroyMagickWand(gif->wand);
  free(gif);
}
