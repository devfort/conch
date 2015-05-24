#include <curses.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

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
  .padding_x = 2,
  .blast_padding = 1,
  .blast_height = 2,
};

enum conch_color {
  NORMAL_COLOR = 1,
  NEW_COLOR = 2,
  SELECTED_COLOR = 3,
};

typedef struct blast_s {
  uint64_t id;
  char *text;
  char *author;
  time_t timestamp;
} blast_s;

typedef void *blast_list_s;
typedef void *screen_state_s;

void render_chrome(WINDOW *window) {
  box(window, 0, 0);
  mvwprintw(window, 0, 3, "conch");
}

void render_blast(WINDOW *window, int y, int x, blast_s *blast, int status_color) {
  mvwvline(window, y, x, ' ' | COLOR_PAIR(status_color), 2);
  mvwprintw(window, y, x + 2,
            blast->text);
  mvwprintw(window, y + 1, x + 2,
            "--%s at %d", blast->author, blast->timestamp);
}

void get_updates(blast_list_s *blasts) {
  // Return if new blasts
}

void render(WINDOW *window, blast_list_s *blasts,
            screen_state_s *current_screen) {

  int max_y = getmaxy(window);

  const int first_blast_y = chrome.padding_y + chrome.border_width;
  const int blast_x = chrome.padding_x + chrome.border_width;

  const int usable_lines = max_y - ((chrome.border_width * 2) + (chrome.padding_y * 2));

  const int max_blasts = usable_lines / (chrome.blast_padding + chrome.blast_height);

  render_chrome(window);

  blast_s blast = {
    .id = 1,
    .text = "This is a blast!",
    .author = "Steve and Alex",
    .timestamp = max_y,
  };

  mvwvline(window, 1, 3, ' ' | COLOR_PAIR(NORMAL_COLOR), max_y - (chrome.border_width * 2));

  int blast_y = first_blast_y;
  for(int i = 0; i < max_blasts; ++i) {
    render_blast(window, blast_y, blast_x, &blast, SELECTED_COLOR);
    blast_y += chrome.blast_padding + chrome.blast_height;
  }

  wrefresh(window);
}
void respond_to_keypresses() {}

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

  if (has_colors()) init_colors();

  // Render status bar and other chrome
  WINDOW *window = newwin(0, 0, 0, 0);
  render_chrome(window);
  wrefresh(window);
  return window;
}

int main(int argc, char **argv) {
  WINDOW *main_window = init_screen();
  blast_list_s blasts = NULL;
  screen_state_s current_screen = NULL;
  while (1) {
    // Poll postgres
    get_updates(blasts);
    // Render screen based on new data
    render(main_window, blasts, current_screen);
    // Respond to keypresses
    respond_to_keypresses();
    // Render screen again
  }
  endwin();
}
