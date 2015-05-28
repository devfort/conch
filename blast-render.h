#ifndef _BLAST_RENDER_H
#define _BLAST_RENDER_H

#include <curses.h>
#include <stdbool.h>

#include "blastlist.h"

#define BLAST_EXTENDED_MARKER " [...]"

typedef struct {
  char **content;
  unsigned int marker_rel_y;
  unsigned int marker_rel_x;
} drawlist;

unsigned int conch_blast_render(WINDOW *window, drawlist *l,
                                unsigned int nlines, int y, int x,
                                unsigned int maxlines, bool invert);
drawlist *conch_blast_prepare(blast *blast, int width, int *nlines);
void conch_drawlist_free(drawlist *l);

#endif /* _BLAST_RENDER_H */
