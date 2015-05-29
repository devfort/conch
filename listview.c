#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <assert.h>

#include "explode.h"
#include "listview.h"
#include "backend.h"
#include "keys.h"
#include "strutils.h"

extern mouthpiece *conn;

listview *conch_listview_new(conch_cli_options const *opts) {
  listview *lv = calloc(1, sizeof(listview));
  lv->stick_to_head = opts->stick_to_head;
  lv->at_head = true;
  lv->page_size = 0;
  lv->username = opts->username;
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

  if (lv->at_head && lv->stick_to_head) {
    conch_listview_jump_to_top(lv);
  }
}

void conch_listview_toggle_stick_to_head(listview *lv) {
  lv->stick_to_head ^= true;
}

void conch_listview_jump_to_top(listview *lv) {
  if (lv->blasts->current == NULL)
    return;
  lv->latest_read = lv->blasts->current = lv->blasts->head;
  lv->top = lv->latest_read;
  lv->render_from_bottom = false;
  lv->at_head = true;
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
  lv->render_from_bottom = false;
  lv->at_head = (lv->blasts->current->prev == NULL);
}

void conch_listview_select_next_blast(listview *lv) {
  if (lv->blasts->current == NULL || lv->blasts->current->next == NULL) {
    return;
  }

  lv->blasts->current = lv->blasts->current->next;

  if (lv->bottom == lv->blasts->current->prev) {
    // We've moved the cursor past the bottom of the screen.
    lv->bottom = lv->blasts->current;
    lv->render_from_bottom = true;
  } else if (lv->bottom == lv->blasts->current && lv->render_overflow) {
    // We've moved the cursor to the last blast, which is partially rendered.
    lv->render_from_bottom = true;
  }

  lv->at_head = false;
}

void conch_listview_jump_to_bottom(listview *lv) {
  if (lv->blasts->current == NULL) {
    return;
  }
  while (lv->bottom->next != NULL) {
    conch_listview_select_next_blast(lv);
  };
}

void conch_listview_select_prev_blast(listview *lv) {
  if (lv->blasts->current == NULL || lv->blasts->current->prev == NULL) {
    return;
  }

  lv->blasts->current = lv->blasts->current->prev;

  if (lv->top == lv->blasts->current->next) {
    // We've moved the cursor past the top of the screen.
    lv->top = lv->blasts->current;
    lv->render_from_bottom = false;
  } else if (lv->top == lv->blasts->current && lv->render_overflow) {
    // We've moved the cursor to the first blast, which is partially rendered.
    lv->render_from_bottom = false;
  }

  // If we were at the latest read blast before moving, update latest read.
  if (lv->latest_read == lv->blasts->current->next) {
    lv->latest_read = lv->blasts->current;
  }

  lv->at_head = (lv->blasts->current->prev == NULL);
}

bool conch_listview_has_unread_blasts(listview *lv) {
  return (lv->latest_read != NULL && lv->latest_read->prev != NULL);
}

blast *conch_listview_find_and_select_blast(listview *lv, const char *term) {
  blast *cur = lv->blasts->current;
  bool past_bottom = false;

  if (cur->next) {
    cur = cur->next;
  } else {
    return false;
  }

  while (cur) {
    char *search_content = strcasestr(cur->content, term);
    char *search_author = strcasestr(cur->user, term);

    if (lv->bottom && cur == lv->bottom->next) {
      past_bottom = true;
    }

    if (search_content || search_author) {
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

  return cur;
}

void conch_listview_create_blast(listview *lv) {
  if (NULL == lv->username) {
    fatal_error("No username set! Restart with --username [you] to post");
  }

  char *content = calloc(1024, sizeof(char));
  int max_y = getmaxy(curscr);
  int max_x = getmaxx(curscr);

  mvhline(max_y - 1, 0, ' ', max_x);
  mvaddstr(max_y - 1, 0, ":");
  move(max_y - 1, 1);
  wrefresh(curscr);

  conch_getstr_input_config();

  getnstr(content, 1024 - 1);

  conch_default_input_config();

  if (strlen(content) != 0) {
    blastresult *posted = conch_blast_post(conn, lv->username, content, NULL);
    if (0 == posted->post) {
      char *error = strclone(posted->error_message);
      conch_blastresult_free(posted);
      fatal_error("Couldn't post blast: %s", error);
    }
  }
}

bool conch_listview_search_forward(listview *lv) {
  if (lv->blasts->current == NULL) {
    return false;
  }
  char *term = calloc(1024, 1);
  int max_y = getmaxy(curscr);
  int max_x = getmaxx(curscr);

  mvhline(max_y - 1, 0, ' ', max_x);
  mvaddstr(max_y - 1, 0, "/");
  move(max_y - 1, 1);
  wrefresh(curscr);

  conch_getstr_input_config();

  getstr(term);

  conch_default_input_config();

  // if search is empty, use the previous one
  if (strlen(term) == 0) {
    conch_listview_repeat_search_forward(lv);
  } else {
    conch_listview_find_and_select_blast(lv, term);
    lv->search_term = term;
  }

  return true;
}

bool conch_listview_repeat_search_forward(listview *lv) {
  if (lv->search_term != NULL) {
    conch_listview_find_and_select_blast(lv, lv->search_term);
  }
  return true;
}

static void compute_page_size(listview *lv) {
  if (lv->blasts->current == NULL) {
    return;
  }
  blast *top = lv->top;
  blast *bottom = lv->bottom;
  int n = 1;
  while (top != bottom) {
    top = top->next;
    n++;
  }
  if (lv->page_size > n) {
    lv->page_size = n;
  }
  if (3 * lv->page_size < 2 * n) {
    lv->page_size = n;
  }
}

void conch_listview_page_down(listview *lv) {
  compute_page_size(lv);
  for (int i = 0; i < lv->page_size; i++) {
    conch_listview_select_next_blast(lv);
  }
}

void conch_listview_page_up(listview *lv) {
  compute_page_size(lv);
  for (int i = 0; i < lv->page_size; i++) {
    conch_listview_select_prev_blast(lv);
  }
}

void conch_listview_free(listview *lv) {
  free(lv);
}
