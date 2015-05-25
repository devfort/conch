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
    lv->latest_read = lv->current_blast = bl;
  }

  if (was_at_top && lv->stick_to_top) {
    conch_listview_jump_to_top(lv);
  }
}

void conch_listview_toggle_stick_to_top(listview *lv) {
  lv->stick_to_top ^= true;
}

void conch_listview_jump_to_top(listview *lv) {
  lv->latest_read = lv->current_blast = lv->head;
  lv->blast_offset = 0;
}

void conch_listview_jump_to_next_unread(listview *lv) {
  if (lv->latest_read == NULL) {
    return;
  }
  if (lv->latest_read->prev) {
    lv->latest_read = lv->current_blast = lv->latest_read->prev;
    lv->blast_offset = 0;
  }
}

void conch_listview_select_next_blast(listview *lv) {
  if (lv->current_blast->next) {
    lv->current_blast = lv->current_blast->next;
  }
}

void conch_listview_select_prev_blast(listview *lv) {
  if (lv->current_blast->prev) {
    bool current_is_latest_read = (lv->latest_read == lv->current_blast);

    lv->current_blast = lv->current_blast->prev;
    if (current_is_latest_read) {
      lv->latest_read = lv->current_blast;
    }
  }
}

bool conch_listview_has_unread_blasts(listview *lv) {
  return (lv->latest_read != NULL && lv->latest_read->prev != NULL);
}

void conch_listview_free(listview *lv) { free(lv); }
