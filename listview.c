#include <stdlib.h>

#include "listview.h"

screen_state_s *conch_listview_new(bool stick_to_top) {
  screen_state_s *lv = calloc(1, sizeof(screen_state_s));
  lv->stick_to_top = stick_to_top;
  return lv;
}

void conch_listview_update(screen_state_s *lv, blastlist *bl) {
  lv->head = bl;
  if (lv->current_blast == NULL) {
    lv->current_blast = bl;
  }
}

void conch_listview_toggle_stick_to_top(screen_state_s *lv) {
  lv->stick_to_top ^= true;
}

void conch_listview_select_next_blast(screen_state_s *lv) {
  if (lv->current_blast->next) {
    lv->current_blast = lv->current_blast->next;
  }
}

void conch_listview_select_prev_blast(screen_state_s *lv) {
  if (lv->current_blast->prev) {
    lv->current_blast = lv->current_blast->prev;
  }
}

void conch_listview_free(screen_state_s *lv) { free(lv); }
