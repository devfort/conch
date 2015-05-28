#ifndef _BLAST_RENDER_H
#define _BLAST_RENDER_H

#include <curses.h>

#include "blastlist.h"

int conch_blast_render(WINDOW *window, char **blast_lines, int y, int x);
char **conch_generate_wrapped_blast(blast *blast, int max_line_length);

#endif /* _BLAST_RENDER_H */
