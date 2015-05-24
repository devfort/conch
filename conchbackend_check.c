#include "checkrunner.h"

#include "conchbackend.h"


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

CONCH_CHECK_MAIN(conchbackend_suite)
