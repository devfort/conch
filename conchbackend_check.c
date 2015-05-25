#include "checkrunner.h"

#include "conchbackend-internal.h"

void assert_valid_result_set(mouthpiece *mp, result_set *rs) {
  ck_assert_ptr_ne(mp, NULL);
  ck_assert_ptr_ne(rs, NULL);
  ck_assert_int_eq(rs->error, 0);
  ck_assert_int_ne(rs->count, 0);
  ck_assert_int_le(rs->count, mp->settings.page_size);
  ck_assert_int_ne(rs->before_token, 0);
  ck_assert_int_ne(rs->after_token, 0);
  for(int i=0; i<rs->count; i++) {
    blast blast = rs->blasts[i];
    ck_assert_ptr_ne(blast.user, NULL);
    ck_assert_ptr_ne(blast.content, NULL);
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
  result_set *recent = conch_recent_blasts(mp);
  assert_valid_result_set(mp, recent);
  conch_free_result_set(recent);
  conch_disconnect(mp);
}
END_TEST

START_TEST(test_can_page_backwards) {
  settings settings = {.page_size = 2 };
  mouthpiece *mp = conch_test_connect(settings);
  result_set *recent = conch_recent_blasts(mp);
  assert_valid_result_set(mp, recent);

  result_set *past = conch_blasts_before(mp, recent->before_token);
  assert_valid_result_set(mp, past);

  conch_free_result_set(recent);
  conch_free_result_set(past);
  conch_disconnect(mp);
}
END_TEST

START_TEST(test_can_page_forwards) {
  settings settings = {.page_size = 2 };
  mouthpiece *mp = conch_test_connect(settings);
  result_set *recent = conch_recent_blasts(mp);
  result_set *past = conch_blasts_before(mp, recent->before_token);

  result_set *back_to_the_future = conch_blasts_after(mp, past->after_token);
  assert_valid_result_set(mp, back_to_the_future);

  conch_free_result_set(recent);
  conch_free_result_set(past);
  conch_free_result_set(back_to_the_future);
  conch_disconnect(mp);
}
END_TEST

START_TEST(test_can_page_forward_one_page) {
  settings settings = {.page_size = 2 };
  mouthpiece *mp = conch_test_connect(settings);
  result_set *recent = conch_recent_blasts(mp);

  result_set *past = conch_blasts_before(mp, recent->before_token);
  result_set *paster = conch_blasts_before(mp, past->before_token);

  result_set *back_to_the_future = conch_blasts_after(mp, paster->after_token);
  assert_valid_result_set(mp, back_to_the_future);

  conch_free_result_set(recent);
  conch_free_result_set(past);
  conch_free_result_set(back_to_the_future);
  conch_disconnect(mp);
}
END_TEST

START_TEST(test_can_silence_everything) {
  settings settings = {.page_size = 10 };
  mouthpiece *mp = conch_test_connect(settings);
  ck_assert_ptr_ne(mp, NULL);
  conch_let_silence_fall(mp);
  result_set *results = conch_recent_blasts(mp);
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
  result_set *results = conch_recent_blasts(mp);
  assert_valid_result_set(mp, results);
  conch_free_result_set(results);
  conch_disconnect(mp);
}
END_TEST

START_TEST(test_free_null_result_set) {
  result_set *results = NULL;
  conch_free_result_set(results);
}
END_TEST

Suite *conchbackend_suite(void) {
  TCase *tc_core = tcase_create("connection");
  tcase_add_test(tc_core, test_can_connect_and_disconnect);
  tcase_add_test(tc_core, test_can_connect_and_disconnect_production);
  TCase *tc_recent = tcase_create("recent");
  tcase_add_test(tc_recent, test_can_retrieve_most_recent);
  TCase *tc_backwards = tcase_create("backwards");
  tcase_add_test(tc_recent, test_can_page_backwards);
  TCase *tc_forwards = tcase_create("forwards");
  tcase_add_test(tc_recent, test_can_page_forwards);
  TCase *tc_forward_one_page = tcase_create("forward_one_page");
  tcase_add_test(tc_recent, test_can_page_forward_one_page);
  TCase *tc_silence = tcase_create("silence");
  tcase_add_test(tc_silence, test_can_silence_everything);
  tcase_add_test(tc_silence, test_rolls_back_tests_on_close);
  TCase *tc_result_set = tcase_create("result_set");
  tcase_add_test(tc_result_set, test_free_null_result_set);

  Suite *s = suite_create("backend");
  suite_add_tcase(s, tc_core);
  suite_add_tcase(s, tc_recent);
  suite_add_tcase(s, tc_backwards);
  suite_add_tcase(s, tc_forwards);
  suite_add_tcase(s, tc_forward_one_page);
  suite_add_tcase(s, tc_silence);
  suite_add_tcase(s, tc_result_set);

  return s;
}

CONCH_CHECK_MAIN(conchbackend_suite)
