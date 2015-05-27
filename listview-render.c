#include <curses.h>
#include <string.h>

#include "blastlist.h"
#include "colors.h"
#include "listview.h"
#include "listview-render.h"
#include "wordwrap.h"

#include "render.h"

static int render_blast(WINDOW *window, int available_width, int y,
                        int gutter_x, blast *blast, chtype highlight) {
  wordwrap_s wrap;
  init_wordwrap(&wrap, blast->content, available_width);

  // Gutter is 1 character wide because we use mvwvline
  const int gutter_width = 1;
  const int blast_x = gutter_x + gutter_width + chrome.blast_left_margin;

  int blast_height = 0;
  for (token_s *token = wordwrap(&wrap); token != NULL;
       token = wordwrap(&wrap)) {
    blast_height = token->y;
    mvwaddnstr(window, y + token->y, blast_x + token->x, token->word,
               token->length);
  }

  blast_height++;

  if (blast->attachment != NULL) {
    mvwprintw(window, y + blast_height, blast_x, "%s", blast->attachment);
    blast_height++;
  }

  mvwprintw(window, y + blast_height, blast_x, "—%s at %s", blast->user,
            blast->posted_at);
  blast_height++;

  mvwvline(window, y, gutter_x, highlight, blast_height);

  return blast_height;
}

static int blast_highlight(blast *blast, listview *lv) {
  if (blast == lv->blasts->current) {
    return ' ' | COLOR_PAIR(SELECTED_COLOR);
  } else {
    return ACS_VLINE | COLOR_PAIR(TIMELINE_COLOR);
  }
}

void conch_listview_render(listview *lv, WINDOW *window, winrect *rect) {

  const int first_blast_y = rect->top, blast_x = rect->left;

  int max_y = getmaxy(window);

  const int usable_lines = rect->bottom - rect->top;
  const int usable_window_width = rect->right - rect->left;

  int available_y = usable_lines;

  if (conch_listview_has_unread_blasts(lv)) {
    conch_status_set("unread blasts");
  }

  mvwvline(window, first_blast_y, blast_x,
           ACS_VLINE | COLOR_PAIR(TIMELINE_COLOR), usable_lines);

  // If we don't have any blasts yet, we return early.
  if (lv->blasts == NULL || lv->blasts->current == NULL) {
    return;
  }

  // Keep track that we are scrolling from the bottom
  bool at_bottom = (lv->bottom == lv->blasts->current);

  blast *blast = lv->top;

  // Indicate that prior blasts are available
  if (blast->prev) {
    mvwvline(window, chrome.border_width, blast_x,
             ACS_VLINE | COLOR_PAIR(NEW_COLOR), 1);
  } else if (lv->stick_to_top) {
    mvwvline(window, chrome.border_width, blast_x,
             ACS_VLINE | COLOR_PAIR(STUCK_COLOR), 1);
  }

  // Loop until we run out of available_y or blasts
  int blast_y = first_blast_y;
  while (1) {
    int blast_height = render_blast(window, usable_window_width, blast_y,
                                    blast_x, blast, blast_highlight(blast, lv));

    blast_y += chrome.blast_padding + blast_height;
    available_y -= chrome.blast_padding + blast_height;

    // See if we hit the end of the screen and exit if we did
    if (available_y <= 0) {
      // If we did, discount the padding so that you can still select the last
      // element on screen
      available_y += chrome.blast_padding;
      break;
    }

    // Exit when we run out of blasts
    if (blast->next) {
      blast = blast->next;
    } else {
      break;
    }
  }

  // Keep track of the last complete blast on screen so that the next blast key
  // press can increment top correctly. Move bottom back up the last fully rendered
  // blast if we overflowed the space.
  lv->bottom = blast;
  if (available_y < 0) {
    lv->bottom = lv->bottom->prev;
  }

  // If we started at the bottom we should remain at the last drawn item.
  if (at_bottom) {
    lv->blasts->current = lv->bottom;
  }

  // Indicate that more blasts are available
  if (blast->next) {
    mvwvline(window, max_y - (2 * chrome.border_width), blast_x,
             ACS_VLINE | COLOR_PAIR(NEW_COLOR), 1);
  }
}
