#ifndef _DETAILVIEW_H
#define _DETAILVIEW_H

#include "blastlist.h"
#include "webfetcher.h"
#include "anigif.h"

typedef struct {
  blastlist *blastlist;
  int line_offset;
  int code_column_offset;

  webfetch_result *attachment;
  anigif *anigif;
} detailview;

detailview *conch_detailview_new(blastlist *blastlist);
void conch_detailview_free(detailview *v);

void conch_detailview_scroll_down(detailview *v);
void conch_detailview_scroll_up(detailview *v);
void conch_detailview_scroll_code_left(detailview *v);
void conch_detailview_scroll_code_right(detailview *v);

void conch_detailview_edit_code_snippet(detailview *v);

#endif /* _DETAILVIEW_H */
