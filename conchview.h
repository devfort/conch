#ifndef _CONCHVIEW_H
#define _CONCHVIEW_H

#include <sys/time.h>
#include <wand/MagickWand.h>

#include "anigif.h"
#include "cli.h"

typedef struct {
  anigif *gif;
  bool started;
} conchview;

conchview *conch_conchview_new(conch_cli_options const *opts);
void conch_conchview_free(conchview *v);

#endif /* _CONCHVIEW_H */
