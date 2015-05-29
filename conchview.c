#include <assert.h>
#include <stdlib.h>
#include <wand/MagickWand.h>

#include "cli.h"
#include "conchview.h"

conchview *conch_conchview_new(conch_cli_options const *opts) {
  conchview *v = calloc(1, sizeof(conchview));

  MagickWandGenesis();
  v->wand = NewMagickWand();
  assert(v->wand);

  MagickBooleanType ret = MagickReadImage(v->wand, "rsrc/piano.gif");
  assert(ret == MagickTrue);

  return v;
}

void conch_conchview_free(conchview *v) {
  if (v == NULL) {
    return;
  }

  v->wand = DestroyMagickWand(v->wand);
  MagickWandTerminus();

  free(v);
}
