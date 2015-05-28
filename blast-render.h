#ifndef _BLAST_RENDER_H
#define _BLAST_RENDER_H

#include <curses.h>

#include "blastlist.h"

#define BLAST_PADDING 2

void conch_blast_render(WINDOW *window, char **blast_lines, int y, int gutter_x,
                        chtype highlight);
char **conch_generate_wrapped_blast(blast *blast, int max_line_length);

#endif /* _BLAST_RENDER_H */
