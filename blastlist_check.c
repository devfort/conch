#include "checkrunner.h"

#include "blastlist.h"
#include "conchbackend.h"


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

CONCH_CHECK_MAIN(blastlist_suite)
