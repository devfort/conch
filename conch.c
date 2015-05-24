#import <stdio.h>
#include <curses.h>

typedef void *blast_list_s;
typedef void *screen_state_s;

void init_screen() {
  initscr();
  cbreak();
  noecho();
  // Render status bar and other chrome
  WINDOW *window = newwin(0, 0, 0, 0);
  box(window, 0, 0);
  mvwprintw(window, 0, 3, "conch");
  wrefresh(window);
}

void get_updates(blast_list_s *blasts) {
  // Return if new blasts
}
void render(blast_list_s *blasts, screen_state_s *current_screen) {}
void respond_to_keypresses() {}

int main(int argc, char **argv) {
  init_screen();
  blast_list_s blasts;
  screen_state_s current_screen;
  while (1) {
    // Poll postgres
    get_updates(blasts);
    // Render screen based on new data
    render(blasts, current_screen);
    // Respond to keypresses
    respond_to_keypresses();
    // Render screen again
  }
  endwin();
}
