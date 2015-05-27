#include <stdlib.h>

#include "listview.h"

listview *conch_listview_new(conch_cli_options const *opts) {
  listview *lv = calloc(1, sizeof(listview));
  lv->stick_to_top = opts->stick_to_top;
  lv->at_top = true;
  return lv;
}

void conch_listview_update(listview *lv, blastlist *bl) {
  lv->blasts = bl;

  if (bl == NULL) {
    lv->latest_read = NULL;
    return;
  }

  if (lv->latest_read == NULL) {
    lv->latest_read = lv->blasts->current;
  }

  if (lv->top == NULL) {
    lv->top = lv->blasts->current;
  }

  if (lv->at_top && lv->stick_to_top) {
    conch_listview_jump_to_top(lv);
  }
}

void conch_listview_toggle_stick_to_top(listview *lv) {
  lv->stick_to_top ^= true;
}

void conch_listview_jump_to_top(listview *lv) {
  lv->latest_read = lv->blasts->current = lv->blasts->head;
  lv->top = lv->latest_read;
  lv->at_top = true;
}

void conch_listview_jump_to_next_unread(listview *lv) {
  if (lv->latest_read == NULL) {
    return;
  }
  if (lv->latest_read->prev) {
    lv->latest_read = lv->blasts->current = lv->latest_read->prev;
    lv->top = lv->latest_read;
  }
  lv->at_top = (lv->blasts->current->prev == NULL);
}

void conch_listview_select_next_blast(listview *lv) {
  if (lv->blasts->current->next) {
    lv->blasts->current = lv->blasts->current->next;
  }
  lv->at_top = (lv->blasts->current->prev == NULL);
}

void conch_listview_select_prev_blast(listview *lv) {
  if (lv->blasts->current->prev) {
    bool current_is_latest_read = (lv->latest_read == lv->blasts->current);

    lv->blasts->current = lv->blasts->current->prev;
    if (current_is_latest_read) {
      lv->latest_read = lv->blasts->current;
    }
  }
  lv->at_top = (lv->blasts->current->prev == NULL);
}

bool conch_listview_has_unread_blasts(listview *lv) {
  return (lv->latest_read != NULL && lv->latest_read->prev != NULL);
}

void conch_listview_free(listview *lv) { free(lv); }
