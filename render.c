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

static unsigned int spinner_state = 0;

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
  " /dev/fort 11 ",
  " |dev|fort 11 ",
  " \\dev\\fort 11 ",
  " -dev-fort 11 ",
  " /dev/fort 11 ",
};
// clang-format on

static const int number_spinner_states = sizeof(spinner) / sizeof(char const *);
#define PROGRESS_SPINNER_START_STATE (number_spinner_states - 5)
static void render_watermark(WINDOW *window, bool spin) {
  int max_x = getmaxx(window);
  int max_y = getmaxy(window) - 1;

  // Spin, either if we've been explicitly instructed to spin, or if we have not
  // completed a whole revolution of the spinner.
  if (spin || spinner_state != (number_spinner_states - 1)) {
    spinner_state = (spinner_state + 1) % number_spinner_states;
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
  if (show_spinner == false && spinner_state == number_spinner_states - 1) {
    spinner_state = PROGRESS_SPINNER_START_STATE;
  }
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

  render_help(window, " i: insert  j/k: down/up  ?: help  q: quit ");

  render_watermark(window, show_spinner);
}
