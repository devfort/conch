#include <curses.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "blastlist.h"

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
};

typedef struct screen_state_s {
  blastlist_item *current_blast;
  int blast_offset;
} screen_state_s;

void render_chrome(WINDOW *window) {
  box(window, 0, 0);
  mvwprintw(window, 0, 3, " conch <@ ");
}

void render_blast(WINDOW *window, int y, int x, blastlist_item *blast,
                  int status_color) {
  mvwvline(window, y, x, ' ' | COLOR_PAIR(status_color), 2);
  mvwprintw(window, y, x + 2, blast->content);
  mvwprintw(window, y + 1, x + 2, "--%s at %d", blast->user, blast->id);
}

void get_updates(blastlist_item *blasts) {
  // Return if new blasts
}

void render(WINDOW *window, screen_state_s *current_screen) {

  int max_y = getmaxy(window);

  const int first_blast_y = chrome.padding_y + chrome.border_width;
  const int blast_x = chrome.padding_x + chrome.border_width;

  const int usable_lines =
      max_y - ((chrome.border_width * 2) + (chrome.padding_y * 2));

  const int max_blasts =
      usable_lines / (chrome.blast_padding + chrome.blast_height);

  werase(window);

  render_chrome(window);

  mvwvline(window, 1, blast_x, ' ' | COLOR_PAIR(NORMAL_COLOR),
           max_y - (chrome.border_width * 2));

  blastlist_item *blast = current_screen->current_blast;

  int blast_y = first_blast_y;
  for(int i = 0; i < max_blasts; ++i) {
    render_blast(window, blast_y, blast_x, blast, SELECTED_COLOR);
    blast_y += chrome.blast_padding + chrome.blast_height;

    blast = blast->next;
  }
  wrefresh(window);
}

int respond_to_keypresses(WINDOW *window, screen_state_s *screen) {
  const int input = wgetch(window);

  switch(input) {
  case KEY_UP:
    break;

  case KEY_DOWN:
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
  init_pair(NORMAL_COLOR, COLOR_WHITE, COLOR_BLACK);
  init_pair(NEW_COLOR, COLOR_BLUE, COLOR_BLUE);
  init_pair(SELECTED_COLOR, COLOR_WHITE, COLOR_RED);
}

WINDOW *init_screen() {
  initscr();
  cbreak();
  noecho();
  refresh();

  if(has_colors())
    init_colors();

  // Render status bar and other chrome
  WINDOW *window = newwin(0, 0, 0, 0);
  render_chrome(window);
  wrefresh(window);
  return window;
}

int main(int argc, char **argv) {
  WINDOW *main_window = init_screen();
  nodelay(main_window, 1);

  blastlist_item temp_blast = {.id = 1,
                               .user = "fort",
                               .content =
                                   "We're going to need a bigger moat.",
                               .next = &temp_blast };

  screen_state_s current_screen = {
    .current_blast = &temp_blast, .blast_offset = 0,
  };

  while(1) {
    // Poll postgres
    // get_updates(blasts);
    // Render screen based on new data
    render(main_window, &current_screen);
    // Respond to keypresses
    respond_to_keypresses(main_window, &current_screen);
    // Render screen again
  }
  endwin();
}
