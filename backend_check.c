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
    blast blast = rs->blasts[i];
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

START_TEST(test_can_connect_and_disconnect_production) {
  settings settings = {.page_size = 10 };
  mouthpiece *mp = conch_connect(settings);
  ck_assert_ptr_ne(mp, NULL);
  conch_disconnect(mp);
}
END_TEST

START_TEST(test_can_retrieve_most_recent) {
  settings settings = {.page_size = 10 };
  mouthpiece *mp = conch_test_connect(settings);
  resultset *recent = conch_recent_blasts(mp);
  assert_valid_resultset(mp, recent);
  conch_free_resultset(recent);
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

  conch_free_resultset(recent);
  conch_free_resultset(past);
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
}
END_TEST

START_TEST(test_can_page_forwards) {
  settings settings = {.page_size = 2 };
  mouthpiece *mp = conch_test_connect(settings);
  resultset *recent = conch_recent_blasts(mp);
  resultset *past = conch_blasts_before(mp, recent->before_token);

  resultset *back_to_the_future = conch_blasts_after(mp, past->after_token);
  assert_valid_resultset(mp, back_to_the_future);

  conch_free_resultset(recent);
  conch_free_resultset(past);
  conch_free_resultset(back_to_the_future);
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

  conch_free_resultset(recent);
  conch_free_resultset(past);
  conch_free_resultset(back_to_the_future);
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
  conch_free_resultset(results);
  conch_disconnect(mp);
}
END_TEST

START_TEST(test_free_null_resultset) {
  resultset *results = NULL;
  conch_free_resultset(results);
}
END_TEST

Suite *backend_suite(void) {
  Suite *s = suite_create("backend");

  ADD_TEST_CASE(s, test_can_connect_and_disconnect);
  ADD_TEST_CASE(s, test_can_connect_and_disconnect_production);
  ADD_TEST_CASE(s, test_can_retrieve_most_recent);
  ADD_TEST_CASE(s, test_can_page_backwards);
  ADD_TEST_CASE(s, test_can_page_forwards);
  ADD_TEST_CASE(s, test_can_page_forward_one_page);
  ADD_TEST_CASE(s, test_can_silence_everything);
  ADD_TEST_CASE(s, test_rolls_back_tests_on_close);
  ADD_TEST_CASE(s, test_free_null_resultset);
  ADD_TEST_CASE(s, test_has_at_least_one_attachment);

  return s;
}

CONCH_CHECK_MAIN(backend_suite)
