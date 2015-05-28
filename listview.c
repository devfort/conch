#include <stdlib.h>
#include <string.h>
#include <curses.h>

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
  if (lv->blasts->current == NULL)
    return;
  lv->latest_read = lv->blasts->current = lv->blasts->head;
  lv->top = lv->latest_read;
  lv->at_top = true;
}

void conch_listview_jump_to_next_unread(listview *lv) {
  if (lv->latest_read == NULL) {
    return;
  }
  if (lv->blasts->current == NULL)
    return;
  if (lv->latest_read->prev) {
    lv->latest_read = lv->blasts->current = lv->latest_read->prev;
    lv->top = lv->latest_read;
  }
  lv->at_top = (lv->blasts->current->prev == NULL);
}

void conch_listview_select_next_blast(listview *lv) {
  if (lv->blasts->current == NULL)
    return;
  if (lv->blasts->current->next) {
    if (lv->bottom == lv->blasts->current) {
      lv->top = lv->top->next;

      if (lv->bottom) {
        lv->bottom = lv->bottom->next;
      }
    }

    lv->blasts->current = lv->blasts->current->next;
  }

  lv->at_top = (lv->blasts->current->prev == NULL);
}

void conch_listview_select_prev_blast(listview *lv) {
  if (lv->blasts->current == NULL)
    return;
  if (lv->blasts->current->prev) {
    bool current_is_latest_read = (lv->latest_read == lv->blasts->current);

    if (lv->top == lv->blasts->current) {
      lv->top = lv->top->prev;

      if (lv->bottom) {
        lv->bottom = lv->bottom->prev;
      }
    }

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

bool conch_listview_search_forward(listview *lv) {
  char *term = calloc(1024, 1);
  int max_y = getmaxy(curscr);
  int max_x = getmaxx(curscr);

  mvhline(max_y - 1, 0, ' ', max_x);
  move(max_y - 1, 0);
  wrefresh(curscr);

  echo();
  curs_set(1);
  getstr(term);
  noecho();

  blast *cur = lv->blasts->current;

  if (cur->next) {
    cur = cur->next;
  } else {
    return true;
  }

  bool past_bottom = false;

  while (cur) {
    char *search = strstr(cur->content, term);

    if (lv->bottom && cur == lv->bottom->next) {
      past_bottom = true;
    }

    if (search != NULL) {
      break;
    }

    cur = cur->next;
  }

  if (cur) {
    if (past_bottom) {
      lv->top = cur;
    }
    lv->blasts->current = cur;
  }

  return true;
}

void conch_listview_free(listview *lv) { free(lv); }
