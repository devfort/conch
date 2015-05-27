#include <curses.h>
#include <string.h>
#include <time.h>

#include "colors.h"

#include "render.h"
#include "listview-render.h"

window_chrome_s chrome = {
  .blast_height = 2,
  .blast_left_margin = 1,
  .blast_padding = 2,
  .border_width = 1,
  .origin_x = 0,
  .origin_y = 0,
  .padding_x = 1,
  .padding_y = 1,
  .title_left_margin = 3,
};

// 64 characters provides space for a 14-character status with the clock
#define MIN_WIDTH_FOR_CLOCK 64

static void render_clock(WINDOW *window, char *clock_text) {
  int max_x = getmaxx(window);
  mvwaddstr(window, chrome.origin_y,
            max_x - strlen(clock_text) - chrome.padding_x, clock_text);
}

static void generate_clock_text(WINDOW *window, int time_str_limit,
                                char *time_str) {
  time_t now = time(NULL);
  struct tm *now_tm = localtime(&now);
  strftime(time_str, time_str_limit, " %Y-%m-%d %H:%M:%S ", now_tm);
}

static void render_help(WINDOW *window, char *help_text) {
  int last_line = getmaxy(window) - 1;
  mvwaddstr(window, last_line, chrome.padding_x, help_text);
}

static void render_chrome(WINDOW *window, char *title_text) {
  int max_x = getmaxx(window);
  int last_line = getmaxy(window) - 1;

  mvwhline(window, chrome.origin_y, chrome.origin_x, ACS_HLINE, max_x);
  mvwhline(window, last_line, chrome.origin_x, ACS_HLINE, max_x);

  mvwaddstr(window, chrome.origin_y, chrome.title_left_margin, title_text);

  const int cursor_invisible = 0;
  curs_set(cursor_invisible);
}

void render_status_message(WINDOW *window, const char *status) {
  int center_offset = (getmaxx(window) - strlen(status) + 2) / 2;
  mvwprintw(window, chrome.origin_y, center_offset, " %s ", status);
}

void render_view(WINDOW *window, view_type current_view, void *view_state) {
  int max_x = getmaxx(window);

  winrect rect = {.top = chrome.padding_y + chrome.border_width,
                  .left = chrome.padding_x + chrome.border_width,
                  .bottom = getmaxy(window) -
                            (chrome.padding_y + chrome.border_width),
                  .right = max_x - (chrome.padding_x + chrome.border_width) };

  werase(window);
  render_chrome(window, " conch 螺");

  if (MIN_WIDTH_FOR_CLOCK <= max_x) {
    char clock_text[1024];
    generate_clock_text(window, sizeof(clock_text), clock_text);
    render_clock(window, clock_text);
  }

  render_help(
      window,
      " j: down  k: up  s: stick to top  0: to top  TAB: to unread  q: quit ");

  switch (current_view) {
  case VIEW_LIST:
    conch_listview_render((listview *)view_state, window, &rect);
    break;
  }
}
