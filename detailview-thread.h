#ifndef _DETAILVIEW_THREAD_H
#define _DETAILVIEW_THREAD_H

#include <stdbool.h>
#include "detailview.h"

#ifdef NO_FETCH_THREAD
void conch_detailview_fetchattachment(detailview *v);
#else
void conch_detailview_spawnfetchthread(detailview *v);
// Return true if the fetch thread has now finished. If it has v->attachment
// and v->anigif might be set
bool conch_detailview_pollfetchthread(detailview *v);
void conch_detailview_killfetchthread(detailview *v);
#endif

#endif
