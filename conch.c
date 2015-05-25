#include <caca.h>
#include <curses.h>
#include <getopt.h>
#include <locale.h>
#include <stdbool.h>
#include <stdlib.h>

#include "blastlist.h"
#include "colors.h"
#include "conchbackend.h"
#include "listview.h"
#include "listview_render.h"
#include "common-image.h"

int respond_to_keypresses(WINDOW *window, listview *lv) {
  const int input = wgetch(window);

  switch (input) {
  case '0':
    conch_listview_jump_to_top(lv);
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
  }

  return input == ERR;
}

void ncurses_write_utf32(WINDOW *window, uint32_t ch) {
  char buf[10];
  int bytes;

  if (ch == CACA_MAGIC_FULLWIDTH)
    return;

  bytes = caca_utf32_to_utf8(buf, ch);
  buf[bytes] = '\0';
  waddstr(window, buf);
}

void ncurses_caca_attrs(int *attr) {
  /* If COLORS == 16, it means the terminal supports full bright colours
   * using setab and setaf (will use \e[90m \e[91m etc. for colours >= 8),
   * we can build 16*16 colour pairs.
   * If COLORS == 8, it means the terminal does not know about bright
   * colours and we need to get them through A_BOLD and A_BLINK (\e[1m
   * and \e[5m). We can only build 8*8 colour pairs. */
  int fg, bg;
  int max = COLORS >= 16 ? 16 : 8;

  for (bg = 0; bg < max; bg++)
    for (fg = 0; fg < max; fg++) {
      /* Use ((max + 7 - fg) % max) instead of fg so that colour 0
       * is light gray on black. Some terminals don't like this
       * colour pair to be redefined. */
      int col = ((max + 7 - fg) % max) + max * bg;
      init_pair(col, curses_colors[fg], curses_colors[bg]);
      attr[fg + 16 * bg] = COLOR_PAIR(col);

      if (max == 8) {
        /* Bright fg on simple bg */
        attr[fg + 8 + 16 * bg] = A_BOLD | COLOR_PAIR(col);
        /* Simple fg on bright bg */
        attr[fg + 16 * (bg + 8)] = A_BLINK | COLOR_PAIR(col);
        /* Bright fg on bright bg */
        attr[fg + 8 + 16 * (bg + 8)] = A_BLINK | A_BOLD | COLOR_PAIR(col);
      }
    }
}

void ncurses_display(WINDOW *window, caca_canvas_t *canvas) {
  int attr[16 * 16];
  int x, y, i;

  ncurses_caca_attrs(&attr[0]);

  for (i = 0; i < caca_get_dirty_rect_count(canvas); i++) {
    uint32_t const *cvchars, *cvattrs;
    int dx, dy, dw, dh;

    caca_get_dirty_rect(canvas, i, &dx, &dy, &dw, &dh);

    cvchars =
        caca_get_canvas_chars(canvas) + dx + dy * caca_get_canvas_width(canvas);
    cvattrs =
        caca_get_canvas_attrs(canvas) + dx + dy * caca_get_canvas_width(canvas);

    for (y = dy; y < dy + dh; y++) {
      wmove(window, y, dx);
      for (x = dx; x < dx + dw; x++) {
        (void)attrset(attr[caca_attr_to_ansi(*cvattrs++)]);
        ncurses_write_utf32(window, *cvchars++);
      }

      cvchars += caca_get_canvas_width(canvas) - dw;
      cvattrs += caca_get_canvas_width(canvas) - dw;
    }
  }
}

void render_conch(WINDOW *window) {
  int lines = getmaxy(window);
  int cols = getmaxx(window);
  WINDOW *cwin = newwin(lines - 1, cols - 1, 1, 1);
  lines = getmaxy(cwin);
  cols = getmaxx(cwin);

  caca_canvas_t *cv = caca_create_canvas(0, 0);
  struct image *i = load_image("CONCH.png");
  caca_add_dirty_rect(cv, 0, 0, cols, lines);
  caca_set_canvas_size(cv, cols - 2, lines - 2);
  caca_set_color_ansi(cv, CACA_DEFAULT, CACA_TRANSPARENT);
  caca_clear_canvas(cv);
  caca_set_dither_algorithm(i->dither, "none");
  caca_dither_bitmap(cv, 0, 0, cols, lines, i->dither, i->pixels);

  ncurses_display(cwin, cv);
  wrefresh(cwin);

  unload_image(i);
  caca_free_canvas(cv);
  delwin(cwin);
}

void init_colors() {
  start_color();
  use_default_colors();

  init_color(100, 333, 333, 333);

  init_pair(NORMAL_COLOR, COLOR_WHITE, COLOR_BLACK);
  init_pair(NEW_COLOR, COLOR_BLUE, COLOR_BLUE);
  init_pair(SELECTED_COLOR, COLOR_WHITE, COLOR_RED);
  init_pair(TIMELINE_COLOR, 100, -1);
  init_pair(STUCK_COLOR, COLOR_GREEN, COLOR_GREEN);
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

  nodelay(window, 1);
  wrefresh(window);

  return window;
}

blastlist *init_blasts(mouthpiece *conn) {
  result_set *result = conch_recent_blasts(conn);
  blastlist *blasts = conch_blastlist_from_result_set(result);
  conch_free_result_set(result);
  return blasts;
}

blastlist *update_blasts(mouthpiece *conn, blastlist *blasts) {
  result_set *result = conch_blasts_after(conn, blasts->id);
  blastlist *newblasts = conch_blastlist_from_result_set(result);
  conch_free_result_set(result);

  return conch_blastlist_join(newblasts, blasts);
}

int main(int argc, char **argv) {
  bool stick_to_top;
  int opt;
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
  } while (conn == NULL);

  blastlist *bl = init_blasts(conn);

  while (1) {
    bl = update_blasts(conn, bl);
    conch_listview_update(lv, bl);
    conch_listview_render(main_window, lv);
    respond_to_keypresses(main_window, lv);
  }

  endwin();
  conch_listview_free(lv);
}
