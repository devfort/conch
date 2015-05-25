#include <stdlib.h>

#include "listview.h"

listview *conch_listview_new(bool stick_to_top) {
  listview *lv = calloc(1, sizeof(listview));
  lv->stick_to_top = stick_to_top;
  return lv;
}

void conch_listview_update(listview *lv, blastlist *bl) {
  bool was_at_top = (lv->head == lv->current_blast);

  lv->head = bl;
  if (lv->current_blast == NULL) {
    lv->current_blast = bl;
  }

  if (was_at_top && lv->stick_to_top) {
    conch_listview_jump_to_top(lv);
  }
}

void conch_listview_toggle_stick_to_top(listview *lv) {
  lv->stick_to_top ^= true;
}

void conch_listview_jump_to_top(listview *lv) {
  lv->current_blast = lv->head;
  lv->blast_offset = 0;
}

void conch_listview_select_next_blast(listview *lv) {
  if (lv->current_blast->next) {
    lv->current_blast = lv->current_blast->next;
  }
}

void conch_listview_select_prev_blast(listview *lv) {
  if (lv->current_blast->prev) {
    lv->current_blast = lv->current_blast->prev;
  }
}

void conch_listview_free(listview *lv) { free(lv); }
