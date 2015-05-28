#ifndef _BLAST_RENDER_H
#define _BLAST_RENDER_H

#include <curses.h>

#include "blastlist.h"

#define BLAST_EXTENDED_MARKER "[@]"

int conch_blast_render(WINDOW *window, char **blast_lines, int y, int x);
char **conch_blast_prepare(blast *blast, int width, int *nlines);

#endif /* _BLAST_RENDER_H */
