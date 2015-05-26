#ifndef _CONCHVIEW_H
#define _CONCHVIEW_H

#include "cli.h"

typedef struct { struct image *imdata; } conchview;

conchview *conch_conchview_new(conch_cli_options const *opts);
void conch_conchview_free(conchview *v);

#endif /* _CONCHVIEW_H */
