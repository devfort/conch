#include <stdlib.h>

#include "conchview.h"
#include "common-image.h"

conchview *conch_conchview_new() {
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
