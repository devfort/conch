#include "checkrunner.h"

#include "blastlist.h"
#include "listview.h"

#define ASSERT_PTR_NULL(ptr) ck_assert_ptr_eq(ptr, NULL)
#define ASSERT_PTR_NOT_NULL(ptr) ck_assert_ptr_ne(ptr, NULL)

START_TEST(test_listview_new_null) {
  blastlist *bl = NULL;
  screen_state_s *lv = conch_listview_new(bl);

  ASSERT_PTR_NOT_NULL(lv);
  ASSERT_PTR_NULL(lv->head);
  ASSERT_PTR_NULL(lv->current_blast);
  ck_assert_int_eq(lv->blast_offset, 0);

  conch_listview_free(lv);
}
END_TEST

Suite *listview_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("listview");
  tc_core = tcase_create("create");
  tcase_add_test(tc_core, test_listview_new_null);
  suite_add_tcase(s, tc_core);

  return s;
}

CONCH_CHECK_MAIN(listview_suite)
