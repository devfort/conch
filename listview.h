#ifndef _LISTVIEW_H
#define _LISTVIEW_H

#include <stdbool.h>
#include "blastlist.h"

typedef struct listview {
  blastlist *head;
  blastlist *current_blast;
  int blast_offset;
  bool stick_to_top;
} listview;

listview *conch_listview_new(bool stick_to_top);
void conch_listview_update(listview *lv, blastlist *bl);
void conch_listview_toggle_stick_to_top(listview *lv);
void conch_listview_jump_to_top(listview *lv);
void conch_listview_select_next_blast(listview *lv);
void conch_listview_select_prev_blast(listview *lv);
void conch_listview_free(listview *lv);

#endif /* _LISTVIEW_H */
