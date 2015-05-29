#include <stdlib.h>
#include <stdarg.h>
#include <curses.h>

#include "explode.h"

void fatal_error(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  endwin();
  vfprintf(stderr, fmt, args);
  fprintf(stderr, "\n");
  va_end(args);
  abort();
}
