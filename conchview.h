#ifndef _CONCHVIEW_H
#define _CONCHVIEW_H

typedef struct { struct image *imdata; } conchview;

conchview *conch_conchview_new();
void conch_conchview_free(conchview *v);

#endif /* _CONCHVIEW_H */
