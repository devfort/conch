#include <curses.h>
#include <stdbool.h>
#include <string.h>

#include "colors.h"

#include "render.h"
#include "config.h"
#include "conchview-render.h"
#include "listview-render.h"
#include "detailview-render.h"

#define ORIGIN_Y 0
#define ORIGIN_X 0

#define CHROME_BORDER_HEIGHT 1
#define CHROME_PADDING_X 1

// Placeholder for status message
#define STATUS_MAXLEN 64
static char status[STATUS_MAXLEN];

// 64 characters provides space for a 14-character status with the clock
#define MIN_WIDTH_FOR_CLOCK 64
#define MAX_LENGTH_FOR_CLOCK 1024

// Should we display the spinner?
static bool show_spinner = false;

static void render_clock(WINDOW *window, char *clock_text) {
  int max_x = getmaxx(window);
  mvwaddstr(window, ORIGIN_Y, max_x - strlen(clock_text) - CHROME_PADDING_X,
            clock_text);
}

static void render_help(WINDOW *window, char *help_text) {
  int last_line = getmaxy(window) - 1;
  mvwaddstr(window, last_line, CHROME_PADDING_X, help_text);
}

static void render_watermark(WINDOW *window, bool spin) {
  int max_x = getmaxx(window);
  int max_y = getmaxy(window) - 1;
  static unsigned int spinner_state;

  // Forgive me father, for I have sinned.
  //
  // - NS 2015-0528
  //
  // clang-format off
  static char const *spinner[] = {
    "              ",
    "              ",
    "              ",
    "             \\",
    "             -",
    "             /",
    "            |d",
    "            \\d",
    "            -d",
    "           /de",
    "           |de",
    "           \\de",
    "          -dev",
    "          /dev",
    "          |dev",
    "         \\dev\\",
    "         -dev-",
    "         /dev/",
    "        |dev|f",
    "        \\dev\\f",
    "        -dev-f",
    "       /dev/fo",
    "       |dev|fo",
    "       \\dev\\fo",
    "      -dev-for",
    "      /dev/for",
    "      |dev|for",
    "     \\dev\\fort",
    "     -dev-fort",
    "     /dev/fort",
    "    |dev|fort ",
    "    \\dev\\fort ",
    "    -dev-fort ",
    "   /dev/fort 1",
    "   |dev|fort 1",
    "   \\dev\\fort 1",
    "  -dev-fort 11",
    "  /dev/fort 11",
    "  |dev|fort 11",
    " \\dev\\fort 11 ",
    " -dev-fort 11 ",
    " /dev/fort 11 ",
    " |dev|fort 11 ",
    " \\dev\\fort 11 ",
    " -dev-fort 11 ",
    " /dev/fort 11 ",
  };
  // clang-format on

  // Spin, either if we've been explicitly instructed to spin, or if we have not
  // completed a whole revolution of the spinner.
  static int const nstates = sizeof(spinner) / sizeof(char const *);
  if (spin || spinner_state != (nstates - 1)) {
    spinner_state = (spinner_state + 1) % nstates;
  }

  mvwaddstr(window, max_y,
            max_x - strlen(spinner[spinner_state]) - CHROME_PADDING_X,
            spinner[spinner_state]);
}

static void render_chrome(WINDOW *window, char *title_text) {
  int max_x = getmaxx(window);
  int last_line = getmaxy(window) - 1;

  mvwhline(window, ORIGIN_Y, ORIGIN_X, ACS_HLINE, max_x);
  mvwhline(window, last_line, ORIGIN_X, ACS_HLINE, max_x);

  mvwaddstr(window, ORIGIN_Y, CHROME_PADDING_X, title_text);
}

void conch_status_clear() {
  status[0] = '\0';
}

void conch_status_set(const char *msg) {
  strncpy(status, msg, STATUS_MAXLEN);
}

void conch_spinner_hide() {
  show_spinner = false;
}
void conch_spinner_show() {
  show_spinner = true;
}

static void render_status_message(WINDOW *window) {
  size_t len = strlen(status);
  if (len == 0) {
    return;
  }
  int center_offset = (getmaxx(window) - len + 2 /* whitespace padding */) / 2;
  mvwprintw(window, ORIGIN_Y, center_offset, " %s ", status);
}

void render_view(WINDOW *window, view_type current_view, void *view_state) {
  int max_x = getmaxx(window);
  int max_y = getmaxy(window);

  if (max_y < 2 * CHROME_BORDER_HEIGHT) {
    mvwaddstr(window, ORIGIN_Y, ORIGIN_X, "Window too small! Embiggen!");
    return;
  }

  // The two -1s here are because ncurses co-ordinates are *inclusive*
  winrect rect = {
    .top = CHROME_BORDER_HEIGHT,
    .left = ORIGIN_X,
    .bottom = max_y - CHROME_BORDER_HEIGHT - 1,
    .right = max_x - 1,
    .width = max_x,
    .height = max_y - 2 * CHROME_BORDER_HEIGHT,
  };

  conch_spinner_hide();
  conch_status_clear();

  switch (current_view) {
  case VIEW_CONCH:
    conch_conchview_render((conchview *)view_state, window, &rect);
    break;
  case VIEW_LIST:
    conch_listview_render((listview *)view_state, window, &rect);
    break;
  case VIEW_DETAIL:
    conch_detailview_render((detailview *)view_state, window, &rect);
    break;
  }

  render_chrome(window, " conch 螺 ");

  if (MIN_WIDTH_FOR_CLOCK <= max_x) {
    char clock_text[MAX_LENGTH_FOR_CLOCK];
    generate_clock_text(MAX_LENGTH_FOR_CLOCK, clock_text);
    render_clock(window, clock_text);
  }

  render_status_message(window);

  render_help(window, " j/k: down/up  s: stick to top  0: to top  TAB: next "
                      "unread  /: search  q: quit ");

  render_watermark(window, show_spinner);
}
