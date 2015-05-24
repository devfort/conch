#include "conchbackend.h"

#include <check.h>

START_TEST(test_can_connect_and_disconnect) {
  settings settings = { .page_size = 10 };
  mouthpiece *mp = conch_connect(settings);
  ck_assert_ptr_ne(mp, (void *)0);
  conch_disconnect(mp);
}
END_TEST

Suite *conchbackend_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("backend");
  tc_core = tcase_create("connection");
  tcase_add_test(tc_core, test_can_connect_and_disconnect);
  suite_add_tcase(s, tc_core);

  return s;
}

int main(void) {
  int number_failed;
  Suite *s;
  SRunner *sr;

  s = conchbackend_suite();
  sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? 0 : 1;
}
