#include "blastlist.h"
#include "conchbackend.h"

#include <check.h>

START_TEST(test_make_empty_result_set) {
  result_set rs = { 0 };
  blastlist *bs = conch_blastlist_new(&rs);

  ck_assert_ptr_ne(bs, (void *)0);
  ck_assert_ptr_eq(bs->head, (void *)0);

  conch_blastlist_free(bs);
}
END_TEST

Suite *blastlist_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("blastlist");
  tc_core = tcase_create("create");
  tcase_add_test(tc_core, test_make_empty_result_set);
  suite_add_tcase(s, tc_core);

  return s;
}

int run_check_suite(Suite *s) {
  int number_failed;
  SRunner *sr;

  sr = srunner_create(s);

  srunner_run_all(sr, CK_VERBOSE);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? 0 : 1;
}

int main(void) { return run_check_suite(blastlist_suite()); }
