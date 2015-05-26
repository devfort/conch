#include <stdlib.h>

#include "detailview.h"

detailview *conch_detailview_new(blastlist *blastlist) {
  detailview *v = calloc(1, sizeof(detailview));
  v->blastlist = blastlist;
  return v;
}

void conch_detailview_free(detailview *v) { free(v); }

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
