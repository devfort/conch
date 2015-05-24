#ifndef _BLASTLIST_H
#define _BLASTLIST_H

#include "conchbackend.h"

typedef struct { blast *head; } blastlist;

blastlist *conch_blastlist_new(result_set *rs);

void conch_blastlist_free(blastlist *bs);

#endif
