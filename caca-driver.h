#ifndef _CACA_DRIVER_H
#define _CACA_DRIVER_H

#include <caca.h>
#include <curses.h>

extern int caca_attr[16 * 16];

void ncurses_write_utf32(WINDOW *window, uint32_t ch);
void ncurses_init_caca_attrs(int *attr);
void mvw_ncurses_display(WINDOW *window, int y, int x, caca_canvas_t *canvas);

#endif /* _CACA_DRIVER_H */
