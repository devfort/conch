#ifndef _LISTVIEW_H
#define _LISTVIEW_H

#include <stdbool.h>
#include "blastlist.h"

typedef struct screen_state_s {
  blastlist *head;
  blastlist *current_blast;
  int blast_offset;
  bool stick_to_top;
} screen_state_s;

screen_state_s *conch_listview_new(bool stick_to_top);
void conch_listview_update(screen_state_s *lv, blastlist *bl);
void conch_listview_toggle_stick_to_top(screen_state_s *lv);
void conch_listview_jump_to_top(screen_state_s *lv);
void conch_listview_select_next_blast(screen_state_s *lv);
void conch_listview_select_prev_blast(screen_state_s *lv);
void conch_listview_free(screen_state_s *lv);

#endif /* _LISTVIEW_H */
