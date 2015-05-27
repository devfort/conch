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

  int max_blasts;
  if (usable_lines == chrome.blast_height) {
    max_blasts = 1;
  } else {
    max_blasts = usable_lines / (chrome.blast_padding + chrome.blast_height);
  }

  if (conch_listview_has_unread_blasts(lv)) {
    conch_status_set("unread blasts");
  }

  mvwvline(window, first_blast_y, blast_x,
           ACS_VLINE | COLOR_PAIR(TIMELINE_COLOR), usable_lines);

  if (0 == max_blasts) {
    mvwaddstr(window, first_blast_y, blast_x + 1,
              "You're gonna need a bigger boat! (Or window.)");
  }

  // If we don't have any blasts yet, we return early.
  if (lv->blasts == NULL || lv->blasts->current == NULL) {
    return;
  }

  blast *blast = lv->blasts->current;

  // Indicate that prior blasts are available
  if (blast->prev) {
    mvwvline(window, chrome.border_width, blast_x,
             ACS_VLINE | COLOR_PAIR(NEW_COLOR), 1);
  } else if (lv->stick_to_top) {
    mvwvline(window, chrome.border_width, blast_x,
             ACS_VLINE | COLOR_PAIR(STUCK_COLOR), 1);
  }

  int blast_y = first_blast_y;
  for (int i = 0; i < max_blasts && available_y > 0; ++i) {
    int blast_height = render_blast(window, usable_window_width, blast_y,
                                    blast_x, blast, blast_highlight(blast, lv));

    blast_y += chrome.blast_padding + blast_height;
    available_y -= blast_height;

    blast = blast->next;
    if (!blast) {
      break;
    }
  }

  // Indicate that more blasts are available
  if (blast) {
    mvwvline(window, max_y - (2 * chrome.border_width), blast_x,
             ACS_VLINE | COLOR_PAIR(NEW_COLOR), 1);
  }
}
