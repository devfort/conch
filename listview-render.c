#include <curses.h>
#include <stdlib.h>
#include <string.h>

#include "blastlist.h"
#include "colors.h"
#include "listview.h"
#include "listview-render.h"
#include "strutils.h"

#include "render.h"

void render_blast(WINDOW *window, char **blast_lines, int y, int gutter_x,
                  chtype highlight) {
  // Gutter is 1 character wide because we use mvwvline
  const int gutter_width = 1;
  const int blast_x = gutter_x + gutter_width + chrome.blast_left_margin;
  int i;
  for (i = 0; blast_lines[i]; i++) {
    mvwaddnstr(window, y + i, blast_x, blast_lines[i], strlen(blast_lines[i]));
  }
  int number_of_blast_lines = i;
  mvwvline(window, y, gutter_x, highlight, number_of_blast_lines);
}

char **generate_wrapped_blast(blast *blast, int max_line_length) {
  char **wrapped_blast = wrap_lines(blast->content, max_line_length);

  int line;
  for (line = 0; wrapped_blast[line]; line++)
    ;

  if (blast->attachment != NULL) {
    char *attachment = malloc(strlen(blast->attachment) + 1);
    strcpy(attachment, blast->attachment);
    wrapped_blast[line] = attachment;
    line++;
  }

  char *attribution_string = malloc(1024);
  sprintf(attribution_string, "—%s at %s", blast->user, blast->posted_at);
  wrapped_blast[line] = attribution_string;

  if (blast->extended) {
    // Show a marker to indicate the blast has a code block with it
    // TODO: Find a better/clearer indicator
    wrapped_blast[line] = strcat(attribution_string, "…");
  }
  line++;

  wrapped_blast[line] = NULL;

  return wrapped_blast;
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

  const int usable_lines = rect->height;
  const int usable_window_width = rect->width;

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
  char **wrapped_blast;
  while (1) {
    wrapped_blast = generate_wrapped_blast(blast, usable_window_width);

    render_blast(window, wrapped_blast, blast_y, blast_x,
                 blast_highlight(blast, lv));
    int blast_height = 0;
    for (int i = 0; wrapped_blast[i]; i++) {
      blast_height++;
    }
    blast_y += chrome.blast_padding + blast_height;
    available_y -= chrome.blast_padding + blast_height;

    // See if we hit the end of the screen and exit if we did
    if (available_y <= 0) {
      // If we did, discount the padding so that you can still select the last
      // element on screen
      available_y += chrome.blast_padding;
      break;
    }

    wrap_lines_free(wrapped_blast);
    // Exit when we run out of blasts
    if (blast->next) {
      blast = blast->next;
    } else {
      break;
    }
  }

  // Keep track of the last complete blast on screen so that the next blast key
  // press can increment top correctly. Move bottom back up the last fully
  // rendered
  // blast if we overflowed the space.
  lv->bottom = blast;
  if (available_y < 0) {
    lv->bottom = lv->bottom->prev;
  }

  // If we started at the bottom we should remain at the last drawn item.
  if (at_bottom && lv->bottom) {
    lv->blasts->current = lv->bottom;
  }

  // Indicate that more blasts are available
  if (blast->next) {
    mvwvline(window, max_y - (2 * chrome.border_width), blast_x,
             ACS_VLINE | COLOR_PAIR(NEW_COLOR), 1);
  }
}
