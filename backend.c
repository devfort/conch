#include <arpa/inet.h>
#include <assert.h>
#include <inttypes.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include "backend-internal.h"
#include "strutils.h"
#include "explode.h"

#define POSTED_DATEFORMAT "'YYYY-MM-DD HH24:MI:SS TZ'"

static mouthpiece *conch_connect_internal(settings settings) {
  PGconn *connection = PQsetdbLogin(settings.host, NULL, NULL, NULL,
                                    settings.database, "bugle", NULL);
  if (PQstatus(connection) == CONNECTION_BAD) {
    fprintf(stderr, "%s", PQerrorMessage(connection));
    return NULL;
  }
  mouthpiece *mp = malloc(sizeof(mouthpiece));
  mp->settings = settings;
  mp->connection = connection;
  mp->is_test = false;
  mp->notification_counter = 0;
  return mp;
}

mouthpiece *conch_connect(settings settings) {
  assert(settings.host);
  assert(settings.database);
  return conch_connect_internal(settings);
}

mouthpiece *conch_local_connect(settings settings) {
  settings.host = "localhost";
  settings.database = "bugle_test";
  return conch_connect_internal(settings);
}

mouthpiece *conch_test_connect(settings settings) {
  mouthpiece *mp = conch_local_connect(settings);
  assert(mp != NULL);
  mp->is_test = true;

  PGresult *res = PQexec(mp->connection, "BEGIN");
  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    fprintf(stderr, "Could not start test transaction: %s",
            PQerrorMessage(mp->connection));
    PQclear(res);
    conch_disconnect(mp);
    return NULL;
  }

  PQclear(res);

  res = PQexec(mp->connection,
               "set transaction isolation level read uncommitted");
  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    fprintf(stderr, "Could not set isolation of test transaction: %s",
            PQerrorMessage(mp->connection));
    PQclear(res);
    conch_disconnect(mp);
    return NULL;
  }

  PQclear(res);

  return mp;
}

void conch_disconnect(mouthpiece *mp) {
  if (mp->is_test) {
    PGresult *res = PQexec(mp->connection, "ROLLBACK");
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
      fprintf(stderr, "Error when rolling back test transaction: %s",
              PQerrorMessage(mp->connection));
    }
    PQclear(res);
  }
  PQfinish(mp->connection);
  free(mp);
}

static void silentNoticeProcessor(void *arg, const char *message) {}
static void defaultNoticeProcessor(void *arg, const char *message) {
  fprintf(stderr, "%s", message);
}

void conch_let_silence_fall(mouthpiece *mp) {
  assert(mp->is_test);
  PQsetNoticeProcessor(mp->connection, silentNoticeProcessor, NULL);
  PGresult *res = PQexec(mp->connection, "truncate table bugle_blast cascade");
  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    fatal_error("Error when truncating test blasts: %s",
                PQerrorMessage(mp->connection));
  }
  PQclear(res);
  PQsetNoticeProcessor(mp->connection, defaultNoticeProcessor, NULL);
}

static uint32_t pg_char_to_int(char *s) {
  return ntohl(*((int32_t *)s));
}

static resultset *pg_result_to_resultset(mouthpiece *mp,
                                         PGresult *query_result) {
  resultset *result = malloc(sizeof(resultset));

  ExecStatusType query_result_status = PQresultStatus(query_result);

  if (query_result_status != PGRES_TUPLES_OK) {
    fprintf(stderr, "Error from DB: %s, %s\n, ", PQerrorMessage(mp->connection),
            PQresStatus(query_result_status));
    result->error = 1;
    result->count = 0;
    result->before_token = 0;
    result->after_token = 0;
  } else {
    assert(PQnfields(query_result) == 6);
    result->error = 0;
    int id_column = PQfnumber(query_result, "id");
    assert(id_column >= 0);
    int name_column = PQfnumber(query_result, "name");
    assert(name_column >= 0);
    int message_column = PQfnumber(query_result, "message");
    assert(message_column >= 0);
    int posted_at_column = PQfnumber(query_result, "posted_at");
    assert(posted_at_column >= 0);
    int attachment_column = PQfnumber(query_result, "attachment");
    assert(attachment_column >= 0);
    int extended_column = PQfnumber(query_result, "extended");
    assert(extended_column >= 0);
    result->count = PQntuples(query_result);
    result->blasts = malloc(sizeof(blastdata) * result->count);

    int n = PQntuples(query_result);

    if (n == 0) {
      PQclear(query_result);
      conch_resultset_free(result);
      return NULL;
    }

    result->after_token =
        pg_char_to_int(PQgetvalue(query_result, 0, id_column));
    result->before_token =
        pg_char_to_int(PQgetvalue(query_result, n - 1, id_column));

    for (int i = 0; i < n; i++) {
      int len = PQgetlength(query_result, i, id_column);
      assert(len == 4);
      uint32_t id = pg_char_to_int(PQgetvalue(query_result, i, id_column));
      blastdata *blast = result->blasts + i;
      blast->id = id;
      blast->user = strclone(PQgetvalue(query_result, i, name_column));
      blast->content = strclone(PQgetvalue(query_result, i, message_column));
      blast->posted_at =
          strclone(PQgetvalue(query_result, i, posted_at_column));
      if (!PQgetlength(query_result, i, attachment_column)) {
        blast->attachment = NULL;
      } else {
        blast->attachment =
            strcopycat("http://bugle.fort/uploads/",
                       PQgetvalue(query_result, i, attachment_column));
      }
      if (!PQgetlength(query_result, i, extended_column)) {
        blast->extended = NULL;
      } else {
        blast->extended =
            strclone(PQgetvalue(query_result, i, extended_column));
      }
    }
  }

  PQclear(query_result);

  return result;
}

resultset *conch_recent_blasts(mouthpiece *mp) {
  char page_size_as_string[6];
  int written = snprintf(page_size_as_string, 6, "%d", mp->settings.page_size);
  assert(written <= 6);

  const char *const params[] = { page_size_as_string };
  Oid paramTypes[] = { 23 };

  PGresult *query_result = PQexecParams(
      mp->connection,
      "select id, message, attachment, extended, "
      "to_char(created, " POSTED_DATEFORMAT ") as posted_at, "
      "(select username from auth_user where auth_user.id = user_id) as name "
      "from bugle_blast order by id desc limit $1::integer;",
      1, paramTypes, params, NULL, NULL,
      true // Ask for result set in binary format rather than text.
      );

  return pg_result_to_resultset(mp, query_result);
}

resultset *conch_blasts_before(mouthpiece *mp, id before_token) {
  char page_size_as_string[6];
  int written = snprintf(page_size_as_string, 6, "%d", mp->settings.page_size);
  assert(written <= 6);
  char max_id_as_string[6];
  written = snprintf(max_id_as_string, 6, "%" PRIid, before_token);
  assert(written <= 6);

  const char *const params[] = { max_id_as_string, page_size_as_string };
  Oid paramTypes[] = { 23, 23 };

  PGresult *query_result = PQexecParams(
      mp->connection,
      "select id, message, attachment, extended, "
      "to_char(created, " POSTED_DATEFORMAT ") as posted_at, "
      "(select username from auth_user where auth_user.id = user_id) as name "
      "from bugle_blast "
      "where id < $1::integer "
      "order by id desc "
      "limit $2::integer ",
      2, paramTypes, params, NULL, NULL,
      true // Ask for result set in binary format rather than text.
      );

  return pg_result_to_resultset(mp, query_result);
}

resultset *conch_blasts_after(mouthpiece *mp, id after_token) {
  char page_size_as_string[6];
  int written = snprintf(page_size_as_string, 6, "%d", mp->settings.page_size);
  assert(written <= 6);
  char max_id_as_string[6];
  written = snprintf(max_id_as_string, 6, "%" PRIid, after_token);
  assert(written <= 6);

  const char *const params[] = { max_id_as_string, page_size_as_string };
  Oid paramTypes[] = { 23, 23 };

  PGresult *query_result = PQexecParams(
      mp->connection,
      "select * from ("
      "select id, message, attachment, extended, "
      "to_char(created, " POSTED_DATEFORMAT ") as posted_at, "
      "(select username from auth_user where auth_user.id = user_id) as name "
      "from bugle_blast "
      "where id > $1::integer "
      "order by id asc "
      "limit $2::integer "
      ") as backwards order by id desc",
      2, paramTypes, params, NULL, NULL,
      true // Ask for result set in binary format rather than text.
      );

  return pg_result_to_resultset(mp, query_result);
}

id user_id_for_username(mouthpiece *mp, char *user) {
  // Return id for username, 0 indicating no such user.
  const char *const params[] = { user };
  PGresult *query_result = PQexecParams(
      mp->connection, "select id from auth_user where username = $1", 1, NULL,
      params, NULL, NULL, true);

  if (PQresultStatus(query_result) != PGRES_TUPLES_OK) {
    fprintf(stderr, "Error when fetching user %s: %s", user,
            PQerrorMessage(mp->connection));
  }
  int n = PQntuples(query_result);
  assert(n <= 1);
  if (!n) {
    PQclear(query_result);
    return 0;
  }
  assert(PQnfields(query_result) == 1);
  id result = pg_char_to_int(PQgetvalue(query_result, 0, 0));
  PQclear(query_result);
  return result;
}

blastresult *conch_blast_post(mouthpiece *mp, char *user, char *content,
                              char *extended) {
  blastresult *result = calloc(1, sizeof(blastresult));
  id user_id = user_id_for_username(mp, user);
  if (!user_id) {
    result->post = 0;
    result->error_message = strcopycat("No user with name ", user);
  } else {
    char user_id_str[20];

    sprintf(user_id_str, "%" PRIid, user_id);
    const char *const params[] = { user_id_str, content, extended };
    PGresult *insert_result = PQexecParams(
        mp->connection, "insert into bugle_blast "
                        "(user_id, message, created, attachment, extended) "
                        "values ($1, $2, now(), '', $3) returning id",
        3, NULL, params, NULL, NULL, true);
    if (PQresultStatus(insert_result) != PGRES_TUPLES_OK) {
      result->post = 0;
      result->error_message = strclone(PQresultErrorMessage(insert_result));
    } else {
      assert(PQnfields(insert_result) == 1);
      assert(PQntuples(insert_result) == 1);
      result->post = pg_char_to_int(PQgetvalue(insert_result, 0, 0));
    }
    PQclear(insert_result);
  }
  return result;
}

void conch_blastresult_free(blastresult *result) {
  if (result == NULL) {
    return;
  }
  if (result->error_message != NULL) {
    free(result->error_message);
  }
  free(result);
}

void conch_resultset_free(resultset *result) {
  if (result == NULL) {
    return;
  }
  for (int i = 0; i < result->count; i++) {
    free(result->blasts[i].content);
    free(result->blasts[i].user);
    free(result->blasts[i].attachment);
    free(result->blasts[i].posted_at);
    free(result->blasts[i].extended);
  }
  free(result->blasts);
  free(result);
}

void conch_notifications_init(notifications *notifications, mouthpiece *mp) {
  notifications->mouthpiece = mp;
  if (!mp->notification_counter) {
    PGresult *listen = PQexec(mp->connection, "listen newblast");
    if (PQresultStatus(listen) != PGRES_COMMAND_OK) {
      fatal_error("Error listening for notifications: %s",
                  PQerrorMessage(mp->connection));
    }
    PQclear(listen);
    mp->notification_counter++;
  }
  notifications->last_notify = mp->notification_counter;
}

bool conch_notifications_poll(notifications *notifications) {
  PGconn *conn = notifications->mouthpiece->connection;
  if (!PQconsumeInput(conn)) {
    fatal_error("Error listening for notifications: %s", PQerrorMessage(conn));
  }
  while (true) {
    PGnotify *notification = PQnotifies(conn);
    if (notification == NULL)
      break;
    PQfreemem(notification);
    notifications->mouthpiece->notification_counter++;
  }
  bool result = notifications->mouthpiece->notification_counter >
                notifications->last_notify;
  notifications->last_notify = notifications->mouthpiece->notification_counter;
  return result;
}

bool conch_notifications_await(notifications *notifications, int timeout_ms) {
  int sleep_left = timeout_ms * 1000;
  int backoff = 1;
  while (sleep_left >= 0) {
    bool result = conch_notifications_poll(notifications);
    if (result)
      return true;
    if (!sleep_left)
      return false;
    usleep(backoff);
    sleep_left -= backoff;
    backoff *= 2;
    if (backoff > sleep_left)
      backoff = sleep_left;
  }
  return false;
}
