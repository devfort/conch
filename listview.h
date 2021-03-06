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
  int page_size;
  bool render_overflow; /* If the last blast to render is only partially drawn
                           (i.e. it overflows the blast render area) the render
                           loop will set this flag. It is used to determine when
                           we should switch render direction. */
  bool render_from_bottom;
  bool stick_to_head;
  bool at_head;
  char *search_term;
  char *username;
} listview;

listview *conch_listview_new(conch_cli_options const *opts);
void conch_listview_update(listview *lv, blastlist *bl);
void conch_listview_toggle_stick_to_head(listview *lv);
void conch_listview_jump_to_bottom(listview *lv);
void conch_listview_jump_to_top(listview *lv);
void conch_listview_jump_to_next_unread(listview *lv);
void conch_listview_select_next_blast(listview *lv);
void conch_listview_select_prev_blast(listview *lv);
bool conch_listview_has_unread_blasts(listview *lv);
void conch_listview_create_blast(listview *lv);
bool conch_listview_search_forward(listview *lv);
bool conch_listview_repeat_search_forward(listview *lv);
void conch_listview_free(listview *lv);
void conch_listview_page_down(listview *lv);
void conch_listview_page_up(listview *lv);

blast *conch_listview_find_and_select_blast(listview *lv, const char *term);

#endif /* _LISTVIEW_H */
