#ifndef _CACA_DRIVER_H
#define _CACA_DRIVER_H
#include <caca.h>
#include <curses.h>

void ncurses_write_utf32(WINDOW *window, uint32_t ch);
void ncurses_caca_attrs(int *attr);
void ncurses_display(WINDOW *window, caca_canvas_t *canvas);

#endif
