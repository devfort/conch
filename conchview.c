#include <stdlib.h>

#include "cli.h"
#include "conchview.h"
#include "common-image.h"

conchview *conch_conchview_new(conch_cli_options const *opts) {
  conchview *v = malloc(sizeof(conchview));
  v->imdata = load_image("rsrc/conch.png");
  return v;
}

void conch_conchview_free(conchview *v) {
  if (v == NULL) {
    return;
  }
  unload_image(v->imdata);
  free(v);
}
