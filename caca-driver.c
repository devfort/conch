#include <caca.h>
#include <curses.h>

#include "colors.h"

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
