#include "conchbackend.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdbool.h>
#include <libpq-fe.h>

struct mouthpiece {
  PGconn *connection;
  settings settings;
};

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

static uint32_t pg_char_to_int(char *s) { return ntohl(*((int32_t *)s)); }

static result_set *pg_result_to_result_set(mouthpiece *mp,
                                           PGresult *query_result) {
  result_set *result = malloc(sizeof(result_set));

  ExecStatusType query_result_status = PQresultStatus(query_result);

  if(query_result_status != PGRES_TUPLES_OK) {
    fprintf(stderr, "Error from DB: %s, %s\n, ", PQerrorMessage(mp->connection),
            PQresStatus(query_result_status));
    result->error = 1;
    result->count = 0;
    result->before_token = 0;
    result->after_token = 0;
  } else {
    assert(PQnfields(query_result) == 3);
    result->error = 0;
    int id_column = PQfnumber(query_result, "id");
    int name_column = PQfnumber(query_result, "name");
    int message_column = PQfnumber(query_result, "message");
    result->count = PQntuples(query_result);
    result->blasts = malloc(sizeof(blast) * result->count);

    int n = PQntuples(query_result);

    assert(n > 0);

    result->after_token =
        pg_char_to_int(PQgetvalue(query_result, 0, id_column));
    result->before_token =
        pg_char_to_int(PQgetvalue(query_result, n - 1, id_column));

    for(int i = 0; i < n; i++) {
      int len = PQgetlength(query_result, i, id_column);
      assert(len == 4);
      uint32_t id = pg_char_to_int(PQgetvalue(query_result, i, id_column));
      blast *blast = result->blasts + i;
      blast->id = id;
      blast->user = strclone(PQgetvalue(query_result, i, name_column));
      blast->content = strclone(PQgetvalue(query_result, i, message_column));
    }
  }

  PQclear(query_result);

  return result;
}

result_set *conch_recent_blasts(mouthpiece *mp) {
  char page_size_as_string[6];
  int written = snprintf(page_size_as_string, 6, "%d", mp->settings.page_size);
  assert(written <= 6);

  const char *const params[] = { page_size_as_string };
  Oid paramTypes[] = { 23 };

  PGresult *query_result = PQexecParams(
      mp->connection,
      "select id, message, "
      "(select username from auth_user where auth_user.id = user_id) as name "
      "from bugle_blast order by id desc limit $1::integer;",
      1, paramTypes, params, NULL, NULL,
      true // Ask for result set in binary format rather than text.
      );

  return pg_result_to_result_set(mp, query_result);
}

result_set *conch_blasts_before(mouthpiece *mp, id before_token) {
  char page_size_as_string[6];
  int written = snprintf(page_size_as_string, 6, "%d", mp->settings.page_size);
  assert(written <= 6);
  char max_id_as_string[6];
  written = snprintf(max_id_as_string, 6, "%lu", before_token);
  assert(written <= 6);

  const char *const params[] = { max_id_as_string, page_size_as_string };
  Oid paramTypes[] = { 23, 23 };

  PGresult *query_result = PQexecParams(
      mp->connection,
      "select id, message, "
      "(select username from auth_user where auth_user.id = user_id) as name "
      "from bugle_blast "
      "where id < $1::integer "
      "order by id desc "
      "limit $2::integer ",
      2, paramTypes, params, NULL, NULL,
      true // Ask for result set in binary format rather than text.
      );

  return pg_result_to_result_set(mp, query_result);
}

void conch_free_result_set(result_set *result) {
  for(int i = 0; i < result->count; i++) {
    free(result->blasts[i].content);
    free(result->blasts[i].user);
  }
  free(result->blasts);
}
