#ifndef _BLAST_RENDER_H
#define _BLAST_RENDER_H

#include <curses.h>
#include <stdbool.h>

#include "blastlist.h"

typedef struct {
  char **content;
  unsigned int nlines; /* number of lines in content */
  bool has_marker;
  unsigned int content_last_line;
  unsigned int last_line_length;
} drawlist;

unsigned int conch_blast_render(WINDOW *window, drawlist *l,
                                unsigned int nlines, int y, int x,
                                unsigned int maxlines, bool invert);
drawlist *conch_blast_prepare(blast *blast, int width, bool display_marker);
void conch_drawlist_free(drawlist *l);

#endif /* _BLAST_RENDER_H */
