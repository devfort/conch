#ifndef _CATS_H
#define _CATS_H

#include <sys/time.h>
#include <wand/MagickWand.h>

typedef struct anigif {
  MagickWand *wand;
  double delay;
  struct timeval last_switch_time;
} anigif;

anigif *anigif_new_from_file(const char *filename);
anigif *anigif_new_from_blob(const void *blob, const size_t length);
void anigif_free(anigif *gif);

#endif
