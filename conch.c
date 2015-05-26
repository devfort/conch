#include <curses.h>
#include <locale.h>
#include <stdlib.h>

#include "backend.h"
#include "blastlist.h"
#include "caca-driver.h"
#include "cli.h"
#include "colors.h"
#include "conchview-render.h"
#include "conchview.h"
#include "keys.h"
#include "listview-render.h"
#include "listview.h"
#include "timeout.h"

// Approximate time to wait between requests to the database (seconds)
#define DB_POLL_INTERVAL 10

// Maximum time to wait for a keypress (tenths of a second)
#define KEY_DELAY 5

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

  // Turn on "half delay" mode, in which getch functions will block for up to n
  // tenths of a second before returning ERR.
  halfdelay(KEY_DELAY);

  wrefresh(window);

  return window;
}

blastlist *init_blasts(mouthpiece *conn) {
  resultset *result = conch_recent_blasts(conn);
  blastlist *blasts = conch_blastlist_from_resultset(result);
  conch_resultset_free(result);
  return blasts;
}

blastlist *update_new_blasts(mouthpiece *conn, blastlist *blast) {
  resultset *result = conch_blasts_after(conn, blast->id);
  blastlist *more_blasts = conch_blastlist_from_resultset(result);
  conch_resultset_free(result);

  return conch_blastlist_join(more_blasts, blast);
}

blastlist *update_old_blasts(mouthpiece *conn, blastlist *blast) {
  resultset *result = conch_blasts_before(conn, blast->id);
  blastlist *more_blasts = conch_blastlist_from_resultset(result);
  conch_resultset_free(result);

  return conch_blastlist_join(blast, more_blasts);
}

mouthpiece *wait_for_connection(settings *config) {
  mouthpiece *conn;
  do {
    conn = conch_connect(*config);
    sleep(1);
  } while (conn == NULL);

  return conn;
}

int main(int argc, char **argv) {
  WINDOW *win;
  blastlist *bl;
  conch_cli_options opts;
  conch_timeout *poll;
  conchview *cv;
  keypress_result res;
  listview *lv;
  mouthpiece *conn;
  settings config = {
    .page_size = 42,
  };

  win = init_screen();
  poll = conch_timeout_new(DB_POLL_INTERVAL);
  opts = conch_parse_command_line_args(argc, argv);

  // Create views
  cv = conch_conchview_new();
  lv = conch_listview_new(opts.stick_to_top);

  // Render conch while loading
  conch_conchview_render(win, cv);

  // Connect to postgres and fetch initial data
  conn = wait_for_connection(&config);
  bl = init_blasts(conn);
  conch_listview_update(lv, bl);
  conch_timeout_reset(poll);

  while (1) {
    if (conch_timeout_expired(poll)) {
      bl = update_new_blasts(conn, bl);
      conch_listview_update(lv, bl);
      conch_timeout_reset(poll);
    }

    conch_listview_render(win, lv);

    if (lv->current_blast->next == NULL) {
      update_old_blasts(conn, lv->current_blast);
    }

    res = conch_keypress_dispatch(wgetch(win), lv);
    if (res == CONCH_EXIT) {
      break;
    }
  }

  endwin();
  conch_conchview_free(cv);
  conch_listview_free(lv);
}
