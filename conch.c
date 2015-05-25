#include <caca.h>
#include <curses.h>
#include <getopt.h>
#include <locale.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include "backend.h"
#include "blastlist.h"
#include "colors.h"
#include "listview.h"
#include "listview_render.h"

// Approximate time to wait between requests to the database (seconds)
#define DB_POLL_INTERVAL 10

// Maximum time to wait for a keypress (tenths of a second)
#define KEY_DELAY 5

static void handle_keypress(const int key, listview *lv) {
  switch (key) {
  case '0':
    conch_listview_jump_to_top(lv);
    break;

  case '.':
  case '\t':
    conch_listview_jump_to_next_unread(lv);
    break;

  case 'j':
    conch_listview_select_next_blast(lv);
    break;

  case 'k':
    conch_listview_select_prev_blast(lv);
    break;

  case 's':
    conch_listview_toggle_stick_to_top(lv);
    break;

  case 'q':
    endwin();
    exit(0);
    break;

  case ERR:
  default:
    break;
  }
}

static bool update_timeout(time_t then, double timeout) {
  time_t now = time(NULL);
  double delta = difftime(now, then);

  return (delta > timeout);
}

WINDOW *init_screen() {
  setlocale(LC_ALL, "");
  initscr();
  cbreak();
  noecho();
  refresh();

  if (has_colors())
    conch_init_colors();

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

int main(int argc, char **argv) {
  bool stick_to_top;
  int opt;
  int key;
  time_t last_update;

  static struct option longopts[] = {
    { "stick-to-top", no_argument, NULL, 's' }, { NULL, 0, NULL, 0 },
  };

  while ((opt = getopt_long(argc, argv, "s", longopts, NULL)) != -1) {
    switch (opt) {
    case 's':
      stick_to_top = true;
      break;
    }
  }
  argc -= optind;
  argv += optind;

  WINDOW *main_window = init_screen();

  // Create new list view and render blank screen
  listview *lv = conch_listview_new(stick_to_top);
  conch_listview_render(main_window, lv);

  // Connect to postgres and fetch blasts
  mouthpiece *conn;
  settings config = {
    .page_size = 42,
  };
  do {
    conn = conch_connect(config);
    sleep(1);
  } while (conn == NULL);

  blastlist *bl = init_blasts(conn);
  conch_listview_update(lv, bl);
  last_update = time(NULL);

  while (1) {
    if (update_timeout(last_update, DB_POLL_INTERVAL)) {
      bl = update_new_blasts(conn, bl);
      conch_listview_update(lv, bl);
      last_update = time(NULL);
    }

    conch_listview_render(main_window, lv);

    if (lv->current_blast->next == NULL) {
      update_old_blasts(conn, lv->current_blast);
    }

    key = wgetch(main_window);
    handle_keypress(key, lv);
  }

  endwin();
  conch_listview_free(lv);
}
