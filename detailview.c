#include <stdlib.h>
#include <string.h>

#include "detailview.h"
#include "anigif.h"

detailview *conch_detailview_new(blastlist *blastlist) {
  detailview *v = calloc(1, sizeof(detailview));
  v->blastlist = blastlist;

  if (blastlist->current->attachment) {
    v->attachment = conch_webfetcher_get(blastlist->current->attachment);

    // If we have an attachment and it's Content-Type from the server is
    // image/* then try to display it
    if (v->attachment &&
        strncmp(v->attachment->content_type, "image/", strlen("image/")) == 0) {
      v->anigif = anigif_new_from_blob(v->attachment->content,
                                       v->attachment->content_length);
    }
  }
  return v;
}

void conch_detailview_free(detailview *v) {
  if (v) {
    conch_webfetch_result_free(v->attachment);
    anigif_free(v->anigif);
  }
  free(v);
}

void conch_detailview_scroll_down(detailview *v) {
  // detailview_render will clamp this for us.
  v->line_offset++;
}

void conch_detailview_scroll_up(detailview *v) {
  if (v->line_offset > 0) {
    v->line_offset--;
  }
}

void conch_detailview_scroll_code_left(detailview *v) {
  if (v->code_column_offset > 0) {
    v->code_column_offset--;
  }
}

void conch_detailview_scroll_code_right(detailview *v) {
  // detailview_render will clamp this for us.
  v->code_column_offset++;
}
