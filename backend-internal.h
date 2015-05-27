#ifndef _BACKEND_INTERNAL_H
#define _BACKEND_INTERNAL_H

#include <libpq-fe.h>
#include <stdbool.h>

#include "backend.h"

struct mouthpiece {
  PGconn *connection;
  settings settings;
  bool is_test;
  int64_t notification_counter;
};

mouthpiece *conch_test_connect(settings settings);
mouthpiece *conch_local_connect(settings settings);

void conch_let_silence_fall(mouthpiece *mp);

id user_id_for_username(mouthpiece *mp, char *user);

#endif /* _BACKEND_INTERNAL_H */
