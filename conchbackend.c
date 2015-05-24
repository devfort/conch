#include "conchbackend.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

mouthpiece *conch_connect(settings settings) {
  PGconn *connection = PQconnectdb("host=core.fort dbname=bugle user=bugle");
  mouthpiece *mp = malloc(sizeof(mouthpiece));
  mp->settings = settings;
  mp->connection = connection;
  return mp;
}

void conch_disconnect(mouthpiece *mp) {
  PQfinish(mp->connection);
  free(mp);
}

static char *strclone(char *c) {
  char *target = malloc(strlen(c) + 1);
  strcpy(target, c);
  return target;
}

result_set *conch_recent_blasts(mouthpiece *mp) {
  result_set *result = malloc(sizeof(result_set));

  PGresult *query_result = PQexecParams(
      mp->connection,
      "select id, message, "
      "(select username from auth_user where auth_user.id = user_id) as name "
      "from bugle_blast order by id desc;",
      0, NULL, NULL, NULL, NULL, 1);

  ExecStatusType query_result_status = PQresultStatus(query_result);

  if (query_result_status != PGRES_TUPLES_OK) {
    fprintf(stderr, "Error from DB: %s, %s\n, ", PQerrorMessage(mp->connection),
            PQresStatus(query_result_status));
    result->error = 1;
    result->count = 0;
    result->before_token = 0;
    result->after_token = 0;
  } else {
    assert(PQnfields(query_result) == 3);

    int id_column = PQfnumber(query_result, "id");
    int name_column = PQfnumber(query_result, "name");
    int message_column = PQfnumber(query_result, "message");
    result->count = PQntuples(query_result);
    result->blasts = malloc(sizeof(blast) * result->count);

    for (int i = 0; i < PQntuples(query_result); i++) {
      int len = PQgetlength(query_result, i, id_column);
      assert(len == 4);
      uint32_t id = ntohl(*((int32_t *)PQgetvalue(query_result, i, id_column)));
      blast *blast = result->blasts + i;
      blast->id = id;
      blast->user = strclone(PQgetvalue(query_result, i, name_column));
      blast->content = strclone(PQgetvalue(query_result, i, message_column));
    }
  }

  PQclear(query_result);

  return result;
}
