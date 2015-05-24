#include <stdlib.h>

#include "blastlist.h"

blastlist *conch_blastlist_new(result_set *rs) {
  blastlist *bs = (blastlist *)malloc(sizeof(blastlist));
  bs->head = NULL;
  return bs;
}

void conch_blastlist_free(blastlist *bs) { free(bs); }
