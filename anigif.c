#include <assert.h>
#include <wand/MagickWand.h>

#include "anigif.h"

// Helper called from both anigif_new_* functions
static void anigif_setup(anigif *gif) {
  gif->delay = MagickGetImageDelay(gif->wand) * 10;

  gettimeofday(&gif->last_switch_time, NULL);
}

anigif *anigif_new_from_file(const char *filename) {
  anigif *gif = calloc(sizeof(anigif), 1);

  gif->wand = NewMagickWand();
  assert(gif->wand);

  MagickBooleanType ret = MagickReadImage(gif->wand, filename);
  assert(ret == MagickTrue);

  anigif_setup(gif);

  return gif;
}

anigif *anigif_new_from_blob(const void *blob, const size_t len) {
  anigif *gif = calloc(sizeof(anigif), 1);

  gif->wand = NewMagickWand();
  assert(gif->wand);

  MagickBooleanType ret = MagickReadImageBlob(gif->wand, blob, len);
  assert(ret == MagickTrue);

  anigif_setup(gif);

  return gif;
}

void anigif_free(anigif *gif) {
  if (gif) {
    gif->wand = DestroyMagickWand(gif->wand);
  }
  free(gif);
}
