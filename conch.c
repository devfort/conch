#include <curses.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

typedef struct blast_s {
  uint64_t id;
  char *text;
  char *author;
  time_t timestamp;
} blast_s;

typedef void *blast_list_s;
typedef void *screen_state_s;

WINDOW *init_screen() {
  initscr();
  cbreak();
  noecho();

  // Render status bar and other chrome
  WINDOW *window = newwin(0, 0, 0, 0);
  box(window, 0, 0);
  mvwprintw(window, 0, 3, "conch");
  wrefresh(window);

  return window;
}

void render_blast(WINDOW *window, int y, int x, blast_s *blast) {
  mvwprintw(window, y, x, blast->text);
  mvwprintw(window, y + 1, x, "--%s at %d", blast->author, blast->timestamp);
}

void get_updates(blast_list_s *blasts) {
  // Return if new blasts
}
void render(WINDOW *window, blast_list_s *blasts,
            screen_state_s *current_screen) {
  blast_s blast = {
    .id = 1,
    .text = "This is a blast!",
    .author = "Steve and Alex",
    .timestamp = time(NULL),
  };
  render_blast(window, 2, 3, &blast);
  wrefresh(window);
}
void respond_to_keypresses() {}

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
