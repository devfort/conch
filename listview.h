#ifndef _LISTVIEW_H
#define _LISTVIEW_H

#include <stdbool.h>

#include "blastlist.h"
#include "cli.h"

typedef struct listview {
  blastlist *blasts;
  blast *latest_read;
  blast *top;
  blast *bottom;
  bool render_from_bottom;
  bool stick_to_top;
  bool at_top;
  char *search_term;
} listview;

listview *conch_listview_new(conch_cli_options const *opts);
void conch_listview_update(listview *lv, blastlist *bl);
void conch_listview_toggle_stick_to_top(listview *lv);
void conch_listview_jump_to_bottom(listview *lv);
void conch_listview_jump_to_top(listview *lv);
void conch_listview_jump_to_next_unread(listview *lv);
void conch_listview_select_next_blast(listview *lv);
void conch_listview_select_prev_blast(listview *lv);
bool conch_listview_has_unread_blasts(listview *lv);
bool conch_listview_search_forward(listview *lv);
bool conch_listview_repeat_search_forward(listview *lv);
void conch_listview_free(listview *lv);

blast *conch_listview_find_and_select_blast(listview *lv, const char *term);

#endif /* _LISTVIEW_H */
