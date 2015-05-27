#ifndef _LISTVIEW_H
#define _LISTVIEW_H

#include <stdbool.h>

#include "blastlist.h"
#include "cli.h"

typedef struct listview {
  blastlist *blasts;
  blast *latest_read;
  blast *top;
  bool stick_to_top;
  bool at_top;
} listview;

listview *conch_listview_new(conch_cli_options const *opts);
void conch_listview_update(listview *lv, blastlist *bl);
void conch_listview_toggle_stick_to_top(listview *lv);
void conch_listview_jump_to_top(listview *lv);
void conch_listview_jump_to_next_unread(listview *lv);
void conch_listview_select_next_blast(listview *lv);
void conch_listview_select_prev_blast(listview *lv);
bool conch_listview_has_unread_blasts(listview *lv);
void conch_listview_free(listview *lv);

#endif /* _LISTVIEW_H */
