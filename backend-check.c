#include "checkrunner.h"

#include "backend-internal.h"

void assert_valid_resultset(mouthpiece *mp, resultset *rs) {
  ck_assert_ptr_ne(mp, NULL);
  ck_assert_ptr_ne(rs, NULL);
  ck_assert_int_eq(rs->error, 0);
  ck_assert_int_ne(rs->count, 0);
  ck_assert_int_le(rs->count, mp->settings.page_size);
  ck_assert_int_ne(rs->before_token, 0);
  ck_assert_int_ne(rs->after_token, 0);
  for (int i = 0; i < rs->count; i++) {
    blastdata blast = rs->blasts[i];
    ck_assert_ptr_ne(blast.user, NULL);
    ck_assert_ptr_ne(blast.content, NULL);
    ck_assert_ptr_ne(blast.posted_at, NULL);
    ck_assert_int_ne(blast.id, 0);
  }
}

START_TEST(test_can_connect_and_disconnect) {
  settings settings = {.page_size = 10 };
  mouthpiece *mp = conch_test_connect(settings);
  ck_assert_ptr_ne(mp, (void *)0);
  conch_disconnect(mp);
}
END_TEST

/*
* Test cannot work off fort
*
START_TEST(test_can_connect_and_disconnect_production) {
  settings settings = {.page_size = 10,
                       .database = "bugle",
                       .host = "bugle.fort" };
  mouthpiece *mp = conch_connect(settings);
  ck_assert_ptr_ne(mp, NULL);
  conch_disconnect(mp);
}
END_TEST
*/

START_TEST(test_can_retrieve_most_recent) {
  settings settings = {.page_size = 10 };
  mouthpiece *mp = conch_test_connect(settings);
  resultset *recent = conch_recent_blasts(mp);
  assert_valid_resultset(mp, recent);
  conch_resultset_free(recent);
  conch_disconnect(mp);
}
END_TEST

START_TEST(test_can_page_backwards) {
  settings settings = {.page_size = 2 };
  mouthpiece *mp = conch_test_connect(settings);
  resultset *recent = conch_recent_blasts(mp);
  assert_valid_resultset(mp, recent);

  resultset *past = conch_blasts_before(mp, recent->before_token);
  assert_valid_resultset(mp, past);

  conch_resultset_free(recent);
  conch_resultset_free(past);
  conch_disconnect(mp);
}
END_TEST

START_TEST(test_has_at_least_one_attachment) {
  settings settings = {.page_size = 100 };
  mouthpiece *mp = conch_test_connect(settings);
  resultset *recent = conch_recent_blasts(mp);
  assert_valid_resultset(mp, recent);
  bool found = false;
  for (int i = 0; i < recent->count; i++) {
    if (recent->blasts[i].attachment != NULL) {
      found = true;
      break;
    }
  }
  ck_assert(found);

  conch_resultset_free(recent);
  conch_disconnect(mp);
}
END_TEST

START_TEST(test_has_at_least_one_extended) {
  settings settings = {.page_size = 100 };
  mouthpiece *mp = conch_test_connect(settings);
  resultset *recent = conch_recent_blasts(mp);
  assert_valid_resultset(mp, recent);
  bool found = false;
  for (int i = 0; i < recent->count; i++) {
    if (recent->blasts[i].extended != NULL) {
      found = true;
      break;
    }
  }
  ck_assert(found);

  conch_resultset_free(recent);
  conch_disconnect(mp);
}
END_TEST

START_TEST(test_can_page_forwards) {
  settings settings = {.page_size = 2 };
  mouthpiece *mp = conch_test_connect(settings);
  resultset *recent = conch_recent_blasts(mp);
  resultset *past = conch_blasts_before(mp, recent->before_token);

  resultset *back_to_the_future = conch_blasts_after(mp, past->after_token);
  assert_valid_resultset(mp, back_to_the_future);

  conch_resultset_free(recent);
  conch_resultset_free(past);
  conch_resultset_free(back_to_the_future);
  conch_disconnect(mp);
}
END_TEST

START_TEST(test_can_page_forward_one_page) {
  settings settings = {.page_size = 2 };
  mouthpiece *mp = conch_test_connect(settings);
  resultset *recent = conch_recent_blasts(mp);

  resultset *past = conch_blasts_before(mp, recent->before_token);
  resultset *paster = conch_blasts_before(mp, past->before_token);

  resultset *back_to_the_future = conch_blasts_after(mp, paster->after_token);
  assert_valid_resultset(mp, back_to_the_future);

  conch_resultset_free(paster);
  conch_resultset_free(recent);
  conch_resultset_free(past);
  conch_resultset_free(back_to_the_future);
  conch_disconnect(mp);
}
END_TEST

START_TEST(test_can_silence_everything) {
  settings settings = {.page_size = 10 };
  mouthpiece *mp = conch_test_connect(settings);

  ck_assert_ptr_ne(mp, NULL);

  conch_let_silence_fall(mp);

  resultset *results = conch_recent_blasts(mp);

  ck_assert_ptr_eq(results, NULL);

  conch_disconnect(mp);
  conch_resultset_free(results);
}
END_TEST

START_TEST(test_rolls_back_tests_on_close) {
  settings settings = {.page_size = 10 };
  mouthpiece *mp = conch_test_connect(settings);
  conch_let_silence_fall(mp);
  conch_disconnect(mp);
  mp = conch_test_connect(settings);
  resultset *results = conch_recent_blasts(mp);
  assert_valid_resultset(mp, results);
  conch_resultset_free(results);
  conch_disconnect(mp);
}
END_TEST

START_TEST(test_free_null_resultset) {
  resultset *results = NULL;
  conch_resultset_free(results);
}
END_TEST

START_TEST(test_blast_post) {
  char *message = "this is my content";
  char *username = "subversion";
  settings settings = {.page_size = 1 };
  mouthpiece *mp = conch_test_connect(settings);

  blastresult *posted = conch_blast_post(mp, username, message, NULL);
  ck_assert_int_ne(posted->post, 0);
  conch_blastresult_free(posted);

  resultset *recent = conch_recent_blasts(mp);
  assert_valid_resultset(mp, recent);
  ck_assert_str_eq(recent->blasts[0].content, message);
  ck_assert_str_eq(recent->blasts[0].user, username);
  ck_assert_ptr_eq(recent->blasts[0].extended, NULL);
  conch_resultset_free(recent);
  conch_disconnect(mp);
}
END_TEST

START_TEST(test_blast_post_rejected_from_invalid_user) {
  char *message = "this is my content";
  char *username = "invalid";
  settings settings = {.page_size = 1 };
  mouthpiece *mp = conch_test_connect(settings);

  blastresult *posted = conch_blast_post(mp, username, message, NULL);
  ck_assert_int_eq(posted->post, 0);
  conch_blastresult_free(posted);

  resultset *recent = conch_recent_blasts(mp);
  assert_valid_resultset(mp, recent);
  ck_assert_str_ne(recent->blasts[0].content, message);
  ck_assert_str_ne(recent->blasts[0].user, username);
  conch_resultset_free(recent);
  conch_disconnect(mp);
}
END_TEST

static void post_or_fail(mouthpiece *mp, char *username, char *message) {
  blastresult *posted = conch_blast_post(mp, username, message, NULL);
  ck_assert_int_ne(posted->post, 0);
  conch_blastresult_free(posted);
}

START_TEST(test_is_notified_on_new_blasts) {
  settings settings = {.page_size = 1 };
  mouthpiece *mp = conch_local_connect(settings);
  mouthpiece *real_mp = conch_local_connect(settings);

  notifications notify;
  conch_notifications_init(&notify, mp);
  ck_assert_int_eq(0, conch_notifications_await(&notify, 500));

  post_or_fail(real_mp, "DRMacIver", "A test message");
  ck_assert_int_eq(1, conch_notifications_poll(&notify));

  conch_disconnect(real_mp);
  conch_disconnect(mp);
}
END_TEST

START_TEST(test_clears_notification_on_read) {
  settings settings = {.page_size = 1 };
  mouthpiece *mp = conch_local_connect(settings);
  mouthpiece *real_mp = conch_local_connect(settings);

  notifications notify;
  conch_notifications_init(&notify, mp);
  ck_assert_int_eq(0, conch_notifications_await(&notify, 200));

  post_or_fail(real_mp, "DRMacIver", "A test message");
  ck_assert_int_eq(1, conch_notifications_await(&notify, 200));
  ck_assert_int_eq(0, conch_notifications_await(&notify, 200));

  conch_disconnect(real_mp);
  conch_disconnect(mp);
}
END_TEST

Suite *backend_suite(void) {
  Suite *s = suite_create("backend");

  /*
  * Test cannot work off fort
  *
  TCase *tc = tcase_create("test_can_connect_and_disconnect_production");
  tcase_set_timeout(tc, 6);
  tcase_add_test(tc, test_can_connect_and_disconnect_production);
  suite_add_tcase(s, tc);
  */

  ADD_TEST_CASE(s, test_can_connect_and_disconnect);
  ADD_TEST_CASE(s, test_can_retrieve_most_recent);
  ADD_TEST_CASE(s, test_can_page_backwards);
  ADD_TEST_CASE(s, test_can_page_forwards);
  ADD_TEST_CASE(s, test_can_page_forward_one_page);
  ADD_TEST_CASE(s, test_can_silence_everything);
  ADD_TEST_CASE(s, test_rolls_back_tests_on_close);
  ADD_TEST_CASE(s, test_free_null_resultset);
  ADD_TEST_CASE(s, test_has_at_least_one_attachment);
  ADD_TEST_CASE(s, test_has_at_least_one_extended);
  ADD_TEST_CASE(s, test_blast_post);
  ADD_TEST_CASE(s, test_blast_post_rejected_from_invalid_user);
  ADD_TEST_CASE(s, test_is_notified_on_new_blasts);
  ADD_TEST_CASE(s, test_clears_notification_on_read);

  return s;
}

CONCH_CHECK_MAIN(backend_suite)
