#include <curses.h>
#include <string.h>
#include <time.h>

#include "blastlist.h"
#include "caca-driver.h"
#include "colors.h"
#include "common-image.h"
#include "listview.h"
#include "listview-render.h"
#include "wordwrap.h"

// 64 characters provides space for a 14-character status with the clock
#define MIN_WIDTH_FOR_CLOCK 64

typedef struct window_chrome_s {
  int border_width;
  int padding_y;
  int padding_x;
  int blast_padding;
  int blast_height;
} window_chrome_s;

window_chrome_s chrome = {
  .border_width = 1,
  .padding_y = 1,
  .padding_x = 1,
  .blast_padding = 1,
  .blast_height = 2,
};

void render_conch(WINDOW *window) {
  int lines = getmaxy(window) - 4;
  int cols = getmaxx(window) - 4;

  struct image *i = load_image("rsrc/conch.png");
  if (i == NULL) {
    // Don't try and render the image if the conch doesn't load.
    return;
  }

  caca_canvas_t *cv = caca_create_canvas(0, 0);
  caca_add_dirty_rect(cv, 0, 0, cols, lines);
  caca_set_canvas_size(cv, cols, lines);
  caca_set_color_ansi(cv, CACA_DEFAULT, CACA_TRANSPARENT);
  caca_clear_canvas(cv);
  caca_set_dither_algorithm(i->dither, "none");
  caca_dither_bitmap(cv, 0, 0, cols, lines, i->dither, i->pixels);

  mvw_ncurses_display(window, 2, 2, cv);

  unload_image(i);
  caca_free_canvas(cv);
}

static void render_clock(WINDOW *window) {
  char time_str[1024];
  time_t now = time(NULL);
  struct tm *now_tm = localtime(&now);
  size_t time_len =
      strftime(time_str, sizeof(time_str), " %Y-%m-%d %H:%M:%S ", now_tm);

  int max_x = getmaxx(window);

  mvwaddstr(window, 0,
            max_x - time_len - chrome.border_width - chrome.padding_x,
            time_str);
}

static void render_help(WINDOW *window) {
  int max_y = getmaxy(window);
  mvwaddstr(
      window, max_y - 1, chrome.padding_x + chrome.border_width,
      " j: down  k: up  s: stick to top  0: to top  TAB: to unread  q: quit ");
}

static void render_chrome(WINDOW *window) {
  box(window, 0, 0);
  mvwaddstr(window, 0, 3, " conch 🐚  ");
  curs_set(0);

  if (MIN_WIDTH_FOR_CLOCK <= getmaxx(window)) {
    render_clock(window);
  }
  render_help(window);
}

static int render_blast(WINDOW *window, int y, int x, blastlist *blast,
                        chtype highlight) {

  int width =
      getmaxx(window) - ((chrome.border_width + chrome.padding_x) * 2) - 2;

  mvwvline(window, y, x, highlight, 2);

  wordwrap_s wrap;
  init_wordwrap(&wrap, blast->content, width);

  int line = 0;
  for (token_s *token = wordwrap(&wrap); token != NULL;
       token = wordwrap(&wrap)) {
    line = token->y;
    mvwaddnstr(window, y + token->y, x + token->x + 2, token->word,
               token->length);
  }

  if (blast->attachment != NULL) {
    mvwprintw(window, y + line + 1, x + 2, "%s", blast->attachment);
    line++;
  }

  mvwprintw(window, y + line + 1, x + 2, "—%s at %s", blast->user,
            blast->posted_at);

  return line + 2;
}

static int blast_highlight(blastlist *blast, listview *lv) {
  if (blast == lv->current_blast) {
    return ' ' | COLOR_PAIR(SELECTED_COLOR);
  } else {
    return ACS_VLINE | COLOR_PAIR(TIMELINE_COLOR);
  }
}

void conch_listview_render(WINDOW *window, listview *lv) {

  int max_y = getmaxy(window);

  const int first_blast_y = chrome.padding_y + chrome.border_width;
  const int blast_x = chrome.padding_x + chrome.border_width;

  const int usable_lines =
      max_y - ((chrome.border_width * 2) + (chrome.padding_y * 2));
  int available_y = usable_lines;

  int max_blasts;
  if (usable_lines == chrome.blast_height) {
    max_blasts = 1;
  } else {
    max_blasts = usable_lines / (chrome.blast_padding + chrome.blast_height);
  }

  werase(window);

  render_chrome(window);

  if (conch_listview_has_unread_blasts(lv)) {
    const char *unread_status = " unread blasts ";
    int center_offset = (getmaxx(window) - strlen(unread_status)) / 2;
    mvwaddstr(window, 0, center_offset, " unread blasts ");
  }

  mvwvline(window, chrome.border_width, blast_x,
           ACS_VLINE | COLOR_PAIR(TIMELINE_COLOR),
           max_y - (chrome.border_width * 2));

  if (0 == max_blasts) {
    mvwaddstr(window, first_blast_y, blast_x + 1,
              "You're gonna need a bigger boat! (Or window.)");
  }

  // If we don't have any blasts yet, we return early.
  if (lv->head == NULL || lv->current_blast == NULL) {
    render_conch(window);
    wrefresh(window);
    return;
  }

  blastlist *blast = lv->current_blast;

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
    int blast_height = render_blast(window, blast_y, blast_x, blast,
                                    blast_highlight(blast, lv));

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
  wmove(window, 0, 11);
  wrefresh(window);
}
