#ifndef _CONCHVIEW_H
#define _CONCHVIEW_H

#include <sys/time.h>
#include <wand/MagickWand.h>

#include "cli.h"

typedef struct {
  MagickWand *wand;
  struct timeval last_render;

  struct image *i;
  bool started;
} conchview;

conchview *conch_conchview_new(conch_cli_options const *opts);
void conch_conchview_free(conchview *v);


#endif /* _CONCHVIEW_H */
