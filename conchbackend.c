#include "conchbackend.h"
#include <stdlib.h>

mouthpiece *conch_connect(settings settings){
  PGconn *connection = PQconnectdb(
    "host=core.fort dbname=bugle user=bugle"
  );
  mouthpiece *mp = malloc(sizeof(mouthpiece));
  mp->settings = settings;
  mp->connection = connection;
  return mp;
}

void conch_disconnect(mouthpiece *mp){
  PQfinish(mp->connection);
  free(mp);
}

