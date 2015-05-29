#include <assert.h>
#include <stdlib.h>

#include "anigif.h"
#include "cli.h"
#include "conchview.h"

conchview *conch_conchview_new(conch_cli_options const *opts) {
  conchview *v = calloc(1, sizeof(conchview));

  v->gif = anigif_new_from_file("rsrc/conch-emoji.png");

  return v;
}

void conch_conchview_free(conchview *v) {
  if (v == NULL) {
    return;
  }

  anigif_free(v->gif);

  free(v);
}
