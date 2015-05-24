#include <locale.h>
#include <curses.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "blastlist.h"
#include "conchbackend.h"

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

enum conch_color {
  NORMAL_COLOR = 0,
  NEW_COLOR = 2,
  SELECTED_COLOR = 3,
  TIMELINE_COLOR = 4,
};

typedef struct screen_state_s {
  blastlist_item *current_blast;
  int blast_offset;
} screen_state_s;

void render_clock(WINDOW *window) {
  char time_str[1024];
  time_t now = time(NULL);
  struct tm *now_tm = localtime(&now);
  size_t time_len =
      strftime(time_str, sizeof(time_str), " %Y-%m-%d %H:%M:%S ", now_tm);

  int max_x = getmaxx(window);

  mvwprintw(window, 0,
            max_x - time_len - chrome.border_width - chrome.padding_x,
            time_str);
}

void render_chrome(WINDOW *window) {
  box(window, 0, 0);
  mvwprintw(window, 0, 3, " conch <@ ");

  render_clock(window);
}

int render_blast(WINDOW *window, int y, int x, blastlist_item *blast, chtype highlight) {
  mvwvline(window, y, x, highlight, 2);
  mvwprintw(window, y, x + 2, blast->content);
  mvwprintw(window, y + 1, x + 2, "--%s at %d", blast->user, blast->id);

  return chrome.blast_height;
}

int blast_highlight(blastlist_item *blast, screen_state_s *screen) {
  if(blast == screen->current_blast) {
    return ' ' | COLOR_PAIR(SELECTED_COLOR);
  } else {
    return ACS_VLINE | COLOR_PAIR(TIMELINE_COLOR);
  }
}

void render(WINDOW *window, screen_state_s *screen) {

  int max_y = getmaxy(window);

  const int first_blast_y = chrome.padding_y + chrome.border_width;
  const int blast_x = chrome.padding_x + chrome.border_width;

  const int usable_lines =
      max_y - ((chrome.border_width * 2) + (chrome.padding_y * 2));

  int max_blasts;

  if(usable_lines == chrome.blast_height) {
    max_blasts = 1;
  } else {
    max_blasts = usable_lines / (chrome.blast_padding + chrome.blast_height);
  }

  werase(window);

  render_chrome(window);

  mvwvline(window, 1, blast_x, ACS_VLINE | COLOR_PAIR(TIMELINE_COLOR),
           max_y - (chrome.border_width * 2));

  if(0 == max_blasts) {
    mvwprintw(window, first_blast_y, blast_x + 1,
              "You're gonna need a bigger boat! (Or window.)");
  }

  blastlist_item *blast = screen->current_blast;

  int blast_y = first_blast_y;
  for(int i = 0; i < max_blasts; ++i) {
    int blast_height =
        render_blast(window, blast_y, blast_x, blast, blast_highlight(blast, screen));

    blast_y += chrome.blast_padding + blast_height;

    if(blast->next) {
      blast = blast->next;
    } else {
      break;
    }
  }
  wrefresh(window);
}

int respond_to_keypresses(WINDOW *window, screen_state_s *screen) {
  const int input = wgetch(window);

  switch(input) {
  case 'j':
    if(screen->current_blast->next) {
      screen->current_blast = screen->current_blast->next;
    }
    break;

  case 'k':
    if(screen->current_blast->prev) {
      screen->current_blast = screen->current_blast->prev;
    }
    break;

  case 'q':
    endwin();
    exit(0);
  }

  return input == ERR;
}

void init_colors() {
  start_color();
  use_default_colors();

  init_color(100, 333, 333, 333);

  init_pair(NORMAL_COLOR, COLOR_WHITE, COLOR_BLACK);
  init_pair(NEW_COLOR, COLOR_BLUE, COLOR_BLUE);
  init_pair(SELECTED_COLOR, COLOR_WHITE, COLOR_RED);
  init_pair(TIMELINE_COLOR, 100, -1);
}

WINDOW *init_screen() {
  setlocale(LC_ALL, "");
  initscr();
  cbreak();
  noecho();
  refresh();

  if(has_colors())
    init_colors();

  // get initial screen setup while we wait for connections
  WINDOW *window = newwin(0, 0, 0, 0);
  render_chrome(window);

  nodelay(window, 1);
  wrefresh(window);

  return window;
}

blastlist *init_blasts(mouthpiece *conn) {
  result_set *result = conch_recent_blasts(conn);
  blastlist *blasts = conch_blastlist_new(result);
  conch_free_result_set(result);
  return blasts;
}

void update_blasts(mouthpiece *conn, blastlist *blasts) {
  result_set *result = conch_blasts_after(conn, blasts->head->id);
  if(!result) {
    return;
  }

  conch_blastlist_insert(blasts, result);
  conch_free_result_set(result);
}

int main(int argc, char **argv) {
  WINDOW *main_window = init_screen();
  mouthpiece *conn;

  settings config = {
    .page_size = 42,
  };
  do {
    conn = conch_connect(config);
  } while (conn == NULL);

  blastlist *blasts = init_blasts(conn);
  screen_state_s screen = {
    .current_blast = blasts->head, .blast_offset = 0,
  };

  while(1) {
    update_blasts(conn, blasts);
    render(main_window, &screen);
    respond_to_keypresses(main_window, &screen);
  }

  endwin();
}
