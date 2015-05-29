#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

#include "anigif.h"
#include "cli.h"
#include "conchview.h"

extern const uint8_t logo_data[];
extern const size_t logo_length;

conchview *conch_conchview_new(conch_cli_options const *opts) {
  conchview *v = calloc(1, sizeof(conchview));

  v->gif = anigif_new_from_blob(logo_data, logo_length);

  return v;
}

void conch_conchview_free(conchview *v) {
  if (v == NULL) {
    return;
  }

  anigif_free(v->gif);

  free(v);
}
