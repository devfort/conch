#include "checkrunner.h"

#include "conchbackend.h"


START_TEST(test_can_connect_and_disconnect) {
  settings settings = { .page_size = 10 };
  mouthpiece *mp = conch_connect(settings);
  ck_assert_ptr_ne(mp, (void *)0);
  conch_disconnect(mp);
}
END_TEST

START_TEST(test_can_retrieve_most_recent) {
  settings settings = { .page_size = 10 };
  mouthpiece *mp = conch_connect(settings);
  result_set *recent = conch_recent_blasts(mp);
  ck_assert_int_eq(recent->error, 0);
  ck_assert_int_ne(recent->count, 0);
  ck_assert_int_le(recent->count, settings.page_size);
  conch_disconnect(mp);
}
END_TEST

Suite *conchbackend_suite(void) {
  TCase *tc_core = tcase_create("connection");
  tcase_add_test(tc_core, test_can_connect_and_disconnect);
  TCase *tc_recent = tcase_create("recent");
  tcase_add_test(tc_recent, test_can_retrieve_most_recent);

  Suite *s = suite_create("backend");
  suite_add_tcase(s, tc_core);
  suite_add_tcase(s, tc_recent);

  return s;
}

CONCH_CHECK_MAIN(conchbackend_suite)
