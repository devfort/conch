#include "checkrunner.h"

#include "blastlist.h"
#include "listview.h"

#define ASSERT_PTR_NULL(ptr) ck_assert_ptr_eq(ptr, NULL)
#define ASSERT_PTR_NOT_NULL(ptr) ck_assert_ptr_ne(ptr, NULL)

START_TEST(test_listview_new) {
  listview *lv = conch_listview_new(false);

  ASSERT_PTR_NOT_NULL(lv);
  ASSERT_PTR_NULL(lv->head);
  ASSERT_PTR_NULL(lv->current_blast);
  ASSERT_PTR_NULL(lv->latest_read);
  ck_assert_int_eq(lv->blast_offset, 0);
  ck_assert_int_eq(lv->stick_to_top, false);
  ck_assert_int_eq(conch_listview_has_unread_blasts(lv), false);

  conch_listview_free(lv);
}
END_TEST

START_TEST(test_listview_new_with_stick_to_top) {
  listview *lv = conch_listview_new(true);

  ASSERT_PTR_NOT_NULL(lv);
  ck_assert_int_eq(lv->stick_to_top, true);

  conch_listview_free(lv);
}
END_TEST

START_TEST(test_listview_update_null_blastlist) {
  blastlist *bl = NULL;
  listview *lv = conch_listview_new(false);

  conch_listview_update(lv, bl);

  ASSERT_PTR_NULL(lv->head);
  ASSERT_PTR_NULL(lv->current_blast);
  ASSERT_PTR_NULL(lv->latest_read);
  ck_assert_int_eq(conch_listview_has_unread_blasts(lv), false);

  conch_listview_free(lv);
}
END_TEST

START_TEST(test_listview_update_sets_current_if_null) {
  blastlist *bl = conch_blastlist_new();
  listview *lv = conch_listview_new(false);

  conch_listview_update(lv, bl);

  // On first update, both head and current_blast should be set to the head of
  // the passed blastlist.
  ck_assert_ptr_eq(lv->head, bl);
  ck_assert_ptr_eq(lv->current_blast, bl);
  ck_assert_ptr_eq(lv->latest_read, bl);
  ck_assert_int_eq(conch_listview_has_unread_blasts(lv), false);

  conch_listview_free(lv);
}
END_TEST

START_TEST(test_listview_update_does_not_set_current_otherwise) {
  blastlist *bl1 = conch_blastlist_new();
  blastlist *bl2 = conch_blastlist_join(conch_blastlist_new(), bl1);
  listview *lv = conch_listview_new(false);

  // If update is called and current_blast is already set, it should not be
  // overridden.
  conch_listview_update(lv, bl1);
  conch_listview_update(lv, bl2);

  ck_assert_ptr_eq(lv->head, bl2);
  ck_assert_ptr_eq(lv->current_blast, bl1);
  ck_assert_ptr_eq(lv->latest_read, bl1);
  ck_assert_int_eq(conch_listview_has_unread_blasts(lv), true);

  conch_listview_free(lv);
}
END_TEST

START_TEST(test_listview_update_jumps_to_top_if_sticky) {
  blastlist *bl = conch_blastlist_new();
  listview *lv = conch_listview_new(true);
  conch_listview_update(lv, bl);

  bl = conch_blastlist_join(conch_blastlist_new(), bl);
  conch_listview_update(lv, bl);
  ck_assert_ptr_eq(lv->current_blast, lv->head);
  ck_assert_ptr_eq(lv->latest_read, lv->head);
  ck_assert_int_eq(conch_listview_has_unread_blasts(lv), false);

  conch_listview_free(lv);
}
END_TEST

START_TEST(test_listview_toggle_stick_to_top) {
  listview *lv = conch_listview_new(true);

  ASSERT_PTR_NOT_NULL(lv);
  ck_assert_int_eq(lv->stick_to_top, true);

  conch_listview_toggle_stick_to_top(lv);
  ck_assert_int_eq(lv->stick_to_top, false);

  conch_listview_toggle_stick_to_top(lv);
  ck_assert_int_eq(lv->stick_to_top, true);
}
END_TEST

START_TEST(test_listview_cursor_movement) {
  blastlist *bl = conch_blastlist_new();
  bl = conch_blastlist_join(conch_blastlist_new(), bl);
  listview *lv = conch_listview_new(false);
  conch_listview_update(lv, bl);

  // We can move forward
  conch_listview_select_next_blast(lv);
  ck_assert_ptr_ne(lv->head, lv->current_blast);

  // We can't move past the end
  blastlist *current = lv->current_blast;
  conch_listview_select_next_blast(lv);
  ck_assert_ptr_eq(current, lv->current_blast);

  // We can move back
  conch_listview_select_prev_blast(lv);
  ck_assert_ptr_eq(lv->head, lv->current_blast);

  // We can't move past the beginning
  conch_listview_select_prev_blast(lv);
  ck_assert_ptr_eq(lv->head, lv->current_blast);
}
END_TEST

START_TEST(test_listview_cursor_movement_updates_latest_read) {
  blastlist *old_bl = conch_blastlist_new();
  old_bl = conch_blastlist_join(conch_blastlist_new(), old_bl);
  old_bl = conch_blastlist_join(conch_blastlist_new(), old_bl);
  listview *lv = conch_listview_new(false);
  conch_listview_update(lv, old_bl);

  // 2 read, now add 2 more unread
  blastlist *new_bl1 = conch_blastlist_join(conch_blastlist_new(), old_bl);
  blastlist *new_bl2 = conch_blastlist_join(conch_blastlist_new(), new_bl1);
  conch_listview_update(lv, new_bl2);
  ck_assert_ptr_eq(lv->latest_read, old_bl);
  ck_assert_int_eq(conch_listview_has_unread_blasts(lv), true);

  // Moving through already-read blasts doesn't change latest_read
  conch_listview_select_next_blast(lv);
  ck_assert_ptr_eq(lv->latest_read, old_bl);
  ck_assert_int_eq(conch_listview_has_unread_blasts(lv), true);
  conch_listview_select_next_blast(lv);
  ck_assert_ptr_eq(lv->latest_read, old_bl);
  ck_assert_int_eq(conch_listview_has_unread_blasts(lv), true);
  conch_listview_select_prev_blast(lv);
  ck_assert_ptr_eq(lv->latest_read, old_bl);
  ck_assert_int_eq(conch_listview_has_unread_blasts(lv), true);
  conch_listview_select_prev_blast(lv);
  ck_assert_ptr_eq(lv->latest_read, old_bl);
  ck_assert_int_eq(conch_listview_has_unread_blasts(lv), true);

  // Moving through unread blasts changes latest_read to current blast
  conch_listview_select_prev_blast(lv);
  ck_assert_ptr_eq(lv->latest_read, new_bl1);
  ck_assert_int_eq(conch_listview_has_unread_blasts(lv), true);
  conch_listview_select_prev_blast(lv);
  ck_assert_ptr_eq(lv->latest_read, new_bl2);
  ck_assert_int_eq(conch_listview_has_unread_blasts(lv), false);

  // Don't set latest_read off the top
  conch_listview_select_prev_blast(lv);
  ck_assert_ptr_eq(lv->latest_read, new_bl2);
  ck_assert_int_eq(conch_listview_has_unread_blasts(lv), false);
}
END_TEST

START_TEST(test_listview_jump_to_top) {
  blastlist *bl = conch_blastlist_new();
  bl = conch_blastlist_join(conch_blastlist_new(), bl);
  bl = conch_blastlist_join(conch_blastlist_new(), bl);
  listview *lv = conch_listview_new(false);
  conch_listview_update(lv, bl);

  conch_listview_select_next_blast(lv);
  conch_listview_select_next_blast(lv);

  ck_assert_ptr_ne(lv->head, lv->current_blast);
  conch_listview_jump_to_top(lv);
  ck_assert_ptr_eq(lv->head, lv->current_blast);
  ck_assert_int_eq(lv->blast_offset, 0);
}
END_TEST

START_TEST(test_listview_jump_to_top_updates_latest_read) {
  blastlist *bl = conch_blastlist_new();
  listview *lv = conch_listview_new(false);
  conch_listview_update(lv, bl);

  // Don't update latest_read on update when stick_to_top disabled
  ck_assert_ptr_eq(lv->latest_read, lv->head);
  bl = conch_blastlist_join(conch_blastlist_new(), bl);
  conch_listview_update(lv, bl);
  ck_assert_ptr_ne(lv->latest_read, lv->head);
  ck_assert_int_eq(conch_listview_has_unread_blasts(lv), true);

  // Update latest_read when jumping to top
  conch_listview_jump_to_top(lv);
  ck_assert_ptr_eq(lv->latest_read, lv->head);
  ck_assert_int_eq(conch_listview_has_unread_blasts(lv), false);
}
END_TEST

START_TEST(test_listview_has_unread_blasts) {
  listview *lv = conch_listview_new(false);

  ck_assert_int_eq(conch_listview_has_unread_blasts(lv), false);

  blastlist *bl = conch_blastlist_new();
  conch_listview_update(lv, bl);
  ck_assert_int_eq(conch_listview_has_unread_blasts(lv), false);

  // Indicate unread blasts when updating
  bl = conch_blastlist_join(conch_blastlist_new(), bl);
  conch_listview_update(lv, bl);
  ck_assert_int_eq(conch_listview_has_unread_blasts(lv), true);
}
END_TEST

Suite *listview_suite(void) {
  Suite *s = suite_create("listview");

  ADD_TEST_CASE(s, test_listview_new);
  ADD_TEST_CASE(s, test_listview_new_with_stick_to_top);
  ADD_TEST_CASE(s, test_listview_update_null_blastlist);
  ADD_TEST_CASE(s, test_listview_update_sets_current_if_null);
  ADD_TEST_CASE(s, test_listview_update_does_not_set_current_otherwise);
  ADD_TEST_CASE(s, test_listview_toggle_stick_to_top);
  ADD_TEST_CASE(s, test_listview_cursor_movement);
  ADD_TEST_CASE(s, test_listview_cursor_movement_updates_latest_read);
  ADD_TEST_CASE(s, test_listview_jump_to_top);
  ADD_TEST_CASE(s, test_listview_update_jumps_to_top_if_sticky);
  ADD_TEST_CASE(s, test_listview_jump_to_top_updates_latest_read);
  ADD_TEST_CASE(s, test_listview_has_unread_blasts);

  return s;
}

CONCH_CHECK_MAIN(listview_suite)
