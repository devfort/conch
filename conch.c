/* Enable sigaction macros on Linux (occasionally) */
#define _XOPEN_SOURCE 600

#include <curses.h>
#include <locale.h>
#include <stdlib.h>
#include <signal.h>

#include "backend.h"
#include "blastlist.h"
#include "caca-driver.h"
#include "cli.h"
#include "colors.h"
#include "keys.h"
#include "conchview-render.h"
#include "conchview.h"
#include "listview.h"
#include "detailview.h"
#include "render.h"

// Approximate time to wait between requests to the database (seconds)
#define DB_POLL_INTERVAL 10

// Maximum time to wait for a keypress (tenths of a second)
#define KEY_DELAY 2

// Duration to show splash screen at startup (tenths of a second)
#define SPLASH_DELAY 20

static bool toggle_conchview;

WINDOW *init_screen() {
  setlocale(LC_ALL, "");
  initscr();
  cbreak();
  noecho();
  refresh();

  conch_init_colors();
  ncurses_init_caca_attrs(&caca_attr[0]);

  // get initial screen setup while we wait for connections
  WINDOW *window = newwin(0, 0, 0, 0);

  // Ask ncurses to give us KEY_LEFT etc, not escape followed by some more keys
  keypad(window, true);

  // Turn on "half delay" mode, in which getch functions will block for up to n
  // tenths of a second before returning ERR.
  halfdelay(KEY_DELAY);

  wrefresh(window);

  return window;
}

void init_blasts(mouthpiece *conn, blastlist *bl) {
  resultset *result = conch_recent_blasts(conn);
  conch_blastlist_prepend_resultset(bl, result);
  conch_resultset_free(result);
}

void update_new_blasts(mouthpiece *conn, blastlist *bl) {
  resultset *result = conch_blasts_after(conn, bl->head->id);
  conch_blastlist_prepend_resultset(bl, result);
  conch_resultset_free(result);
}

void update_old_blasts(mouthpiece *conn, blastlist *bl) {
  resultset *result = conch_blasts_before(conn, bl->tail->id);
  conch_blastlist_append_resultset(bl, result);
  conch_resultset_free(result);
}

static void handle_magic_message_from_the_operator(int signal) {
  toggle_conchview = true;
}

static void bind_signal_handlers() {
  struct sigaction act, oact;

  act.sa_handler = handle_magic_message_from_the_operator;
  sigemptyset(&act.sa_mask);
  act.sa_flags = SA_RESTART;

  sigaction(SIGUSR1, &act, &oact);
}

mouthpiece *wait_for_connection(settings *config) {
  mouthpiece *conn;
  while (1) {
    conn = conch_connect(*config);
    if (conn != NULL) {
      break;
    }
    sleep(1);
  }
  return conn;
}

int main(int argc, char **argv) {
  WINDOW *win;
  blastlist *bl;
  conch_cli_options opts;
  notifications notifications;
  conchview *cv;
  keypress_result res;
  listview *lv;
  mouthpiece *conn;
  view_type current_view;
  void *current_view_state;
  int splash_display_cycles = SPLASH_DELAY / KEY_DELAY;
  bool started = false;
  settings config = {
    .page_size = 42,
  };

  win = init_screen();
  opts = conch_parse_command_line_args(argc, argv);
  config.host = opts.host;
  config.database = opts.database;

  // Create views
  cv = conch_conchview_new(&opts);
  lv = conch_listview_new(&opts);

  // Render conch while loading
  current_view = VIEW_CONCH;
  current_view_state = cv;
  render_view(win, current_view, current_view_state);

  bind_signal_handlers();

  // Connect to postgres and fetch initial data
  conn = wait_for_connection(&config);
  conch_notifications_init(&notifications, conn);
  bl = conch_blastlist_new();
  init_blasts(conn, bl);
  conch_listview_update(lv, bl);

  detailview *dv = conch_detailview_new(bl);

  while (1) {
    if (!started) {
      if (splash_display_cycles > 0) {
        splash_display_cycles--;
      } else {
        cv->started = true;
        started = true;
        current_view = VIEW_LIST;
        current_view_state = lv;
      }
    }

    if (conch_notifications_poll(&notifications)) {
      if (bl->current == NULL) {
        init_blasts(conn, bl);
      } else {
        update_new_blasts(conn, bl);
      }
      conch_listview_update(lv, bl);
    }

    if (toggle_conchview) {
      toggle_conchview = false;
      ungetch('@');
    }

    render_view(win, current_view, current_view_state);

    if (bl->current != NULL && bl->current->next == NULL) {
      update_old_blasts(conn, bl);
    }

    res =
        conch_keypress_dispatch(wgetch(win), current_view, current_view_state);
    switch (res) {
    case CONCH_NOP:
      break;
    case CONCH_EXIT:
      goto terminate;
    case CONCH_CONCH:
      current_view = VIEW_CONCH;
      current_view_state = cv;
      break;
    case CONCH_DETAIL:
      current_view = VIEW_DETAIL;
      current_view_state = dv;
      break;
    case CONCH_LIST:
      current_view = VIEW_LIST;
      current_view_state = lv;
      break;
    }
  }

terminate:
  endwin();
  conch_blastlist_free(bl);
  conch_conchview_free(cv);
  conch_detailview_free(dv);
  conch_listview_free(lv);
}
