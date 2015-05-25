#ifndef _CONCHBACKEND_INTERNAL_H
#define _CONCHBACKEND_INTERNAL_H

#include "conchbackend.h"
#include <libpq-fe.h>
#include <stdbool.h>

struct mouthpiece {
  PGconn *connection;
  settings settings;
  bool is_test;
};

mouthpiece *conch_test_connect(settings settings);

void conch_let_silence_fall(mouthpiece *mp);

#endif /* _CONCHBACKEND_INTERNAL_H */
