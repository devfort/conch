#include "checkrunner.h"

#include "conchbackend.h"

START_TEST(test_can_connect_and_disconnect) {
  settings settings = {.page_size = 10 };
  mouthpiece *mp = conch_connect(settings);
  ck_assert_ptr_ne(mp, NULL);
  conch_disconnect(mp);
}
END_TEST

START_TEST(test_can_retrieve_most_recent) {
  settings settings = {.page_size = 10 };
  mouthpiece *mp = conch_connect(settings);
  result_set *recent = conch_recent_blasts(mp);
  ck_assert_int_eq(recent->error, 0);
  ck_assert_int_ne(recent->count, 0);
  ck_assert_int_le(recent->count, settings.page_size);
  ck_assert_int_ne(recent->before_token, 0);
  ck_assert_int_ne(recent->after_token, 0);
  conch_free_result_set(recent);
  conch_disconnect(mp);
}
END_TEST

START_TEST(test_can_page_backwards) {
  settings settings = {.page_size = 2 };
  mouthpiece *mp = conch_connect(settings);
  result_set *recent = conch_recent_blasts(mp);
  ck_assert_int_eq(recent->error, 0);
  ck_assert_int_eq(recent->count, 2);
  ck_assert_int_ne(recent->before_token, 0);
  ck_assert_int_ne(recent->after_token, 0);

  result_set *past = conch_blasts_before(mp, recent->before_token);
  ck_assert_int_eq(past->error, 0);
  ck_assert_int_eq(past->count, 2);
  ck_assert_int_ne(past->before_token, 0);
  ck_assert_int_ne(past->after_token, 0);

  conch_free_result_set(recent);
  conch_free_result_set(past);
  conch_disconnect(mp);
}
END_TEST

START_TEST(test_can_page_forwards) {
  settings settings = {.page_size = 2 };
  mouthpiece *mp = conch_connect(settings);
  result_set *recent = conch_recent_blasts(mp);
  result_set *past = conch_blasts_before(mp, recent->before_token);

  result_set *back_to_the_future = conch_blasts_after(mp, past->after_token);
  ck_assert_int_eq(back_to_the_future->error, 0);
  ck_assert_int_eq(back_to_the_future->count, 2);
  ck_assert_int_eq(back_to_the_future->before_token, recent->before_token);
  ck_assert_int_eq(back_to_the_future->after_token, recent->after_token);

  conch_free_result_set(recent);
  conch_free_result_set(past);
  conch_free_result_set(back_to_the_future);
  conch_disconnect(mp);
}
END_TEST

START_TEST(test_can_page_forward_one_page) {
  settings settings = {.page_size = 2 };
  mouthpiece *mp = conch_connect(settings);
  result_set *recent = conch_recent_blasts(mp);

  result_set *past = conch_blasts_before(mp, recent->before_token);
  result_set *paster = conch_blasts_before(mp, past->before_token);

  result_set *back_to_the_future = conch_blasts_after(mp, paster->after_token);
  ck_assert_int_eq(back_to_the_future->error, 0);
  ck_assert_int_eq(back_to_the_future->count, 2);
  ck_assert_int_eq(back_to_the_future->before_token, past->before_token);
  ck_assert_int_eq(back_to_the_future->after_token, past->after_token);

  conch_free_result_set(recent);
  conch_free_result_set(past);
  conch_free_result_set(back_to_the_future);
  conch_disconnect(mp);
}
END_TEST

Suite *conchbackend_suite(void) {
  TCase *tc_core = tcase_create("connection");
  tcase_add_test(tc_core, test_can_connect_and_disconnect);
  TCase *tc_recent = tcase_create("recent");
  tcase_add_test(tc_recent, test_can_retrieve_most_recent);
  TCase *tc_backwards = tcase_create("backwards");
  tcase_add_test(tc_recent, test_can_page_backwards);
  TCase *tc_forwards = tcase_create("forwards");
  tcase_add_test(tc_recent, test_can_page_forwards);
  TCase *tc_forward_one_page = tcase_create("forward_one_page");
  tcase_add_test(tc_recent, test_can_page_forward_one_page);

  Suite *s = suite_create("backend");
  suite_add_tcase(s, tc_core);
  suite_add_tcase(s, tc_recent);
  suite_add_tcase(s, tc_backwards);
  suite_add_tcase(s, tc_forwards);
  suite_add_tcase(s, tc_forward_one_page);

  return s;
}

CONCH_CHECK_MAIN(conchbackend_suite)
