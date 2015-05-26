#ifndef _DETAILVIEW_RENDER_H
#define _DETAILVIEW_RENDER_H

#include <curses.h>

#include "detailview.h"
#include "render.h"

void conch_detailview_render(detailview *v, WINDOW *w, winrect *rect);

#endif /* _DETAILVIEW_RENDER_H */
