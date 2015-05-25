#include <curses.h>
#include <time.h>

#include "blastlist.h"
#include "colors.h"
#include "listview.h"
#include "listview_render.h"
#include "wordwrap.h"

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
  mvwaddstr(window, max_y - 1, chrome.padding_x + chrome.border_width,
            " j: down  k: up  s: stick to top  0: jump to top  q: quit ");
}

static void render_chrome(WINDOW *window) {
  box(window, 0, 0);
  mvwaddstr(window, 0, 3, " conch <@ ");

  render_clock(window);
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

  mvwprintw(window, y + line + 1, x + 2, "--%s at %s", blast->user,
            blast->posted_at);

  return line + 2;
}

static int blast_highlight(blastlist *blast, screen_state_s *screen) {
  if (blast == screen->current_blast) {
    return ' ' | COLOR_PAIR(SELECTED_COLOR);
  } else {
    return ACS_VLINE | COLOR_PAIR(TIMELINE_COLOR);
  }
}

void conch_listview_render(WINDOW *window, screen_state_s *screen) {

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

  mvwvline(window, chrome.border_width, blast_x,
           ACS_VLINE | COLOR_PAIR(TIMELINE_COLOR),
           max_y - (chrome.border_width * 2));

  if (0 == max_blasts) {
    mvwaddstr(window, first_blast_y, blast_x + 1,
              "You're gonna need a bigger boat! (Or window.)");
  }

  blastlist *blast = screen->current_blast;

  // Indicate that prior blasts are available
  if (blast->prev) {
    mvwvline(window, chrome.border_width, blast_x,
             ACS_VLINE | COLOR_PAIR(NEW_COLOR), 1);
  } else if (screen->stick_to_top) {
    mvwvline(window, chrome.border_width, blast_x,
             ACS_VLINE | COLOR_PAIR(STUCK_COLOR), 1);
  }

  int blast_y = first_blast_y;
  for (int i = 0; i < max_blasts && available_y > 0; ++i) {
    int blast_height = render_blast(window, blast_y, blast_x, blast,
                                    blast_highlight(blast, screen));

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
  wrefresh(window);
}
