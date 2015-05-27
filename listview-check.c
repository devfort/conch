#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "checkrunner.h"

#include "blastlist.h"
#include "cli.h"
#include "listview.h"

#define ASSERT_PTR_NULL(ptr) ck_assert_ptr_eq(ptr, NULL)
#define ASSERT_PTR_NOT_NULL(ptr) ck_assert_ptr_ne(ptr, NULL)

/*
 * Make a new empty fixture blast with the specified id.
 */
blast *blast_fixture_new(id id) {
  blast *b = calloc(1, sizeof(blast));

  if (b == NULL) {
    fprintf(stderr, "conch_blast_new: could not alloc\n");
    abort();
  }

  b->id = id;

  return b;
}

/*
 * Make a new fixture blastlist of the specified length. All blasts in the
 * blastlist will be empty (zero-valued) and if n > 0 the current blast will
 * point to the head.
 */
blastlist *blastlist_fixture_new(int n) {
  int i = 0;
  blastlist *bl = calloc(1, sizeof(blastlist));

  if (bl == NULL) {
    fprintf(stderr, "blastlist_fixture_new: could not alloc\n");
    abort();
  }

  if (n == 0) {
    return bl;
  }

  blast *cur = blast_fixture_new((id)i);
  bl->head = cur;
  bl->current = cur;

  for (i = 1; i < n; i++) {
    cur->next = blast_fixture_new((id)i);
    cur->next->prev = cur;
    cur = cur->next;
  }

  bl->tail = cur;

  return bl;
}

START_TEST(test_listview_new) {
  conch_cli_options opts = {.stick_to_top = false };
  listview *lv = conch_listview_new(&opts);

  ASSERT_PTR_NOT_NULL(lv);
  ASSERT_PTR_NULL(lv->blasts);
  ASSERT_PTR_NULL(lv->latest_read);
  ck_assert_int_eq(lv->stick_to_top, false);
  ck_assert_int_eq(conch_listview_has_unread_blasts(lv), false);

  conch_listview_free(lv);
}
END_TEST

START_TEST(test_listview_new_with_stick_to_top) {
  conch_cli_options opts = {.stick_to_top = true };
  listview *lv = conch_listview_new(&opts);

  ASSERT_PTR_NOT_NULL(lv);
  ck_assert_int_eq(lv->stick_to_top, true);

  conch_listview_free(lv);
}
END_TEST

START_TEST(test_listview_update_null_blastlist) {
  blastlist *bl = NULL;
  conch_cli_options opts = {.stick_to_top = false };
  listview *lv = conch_listview_new(&opts);

  conch_listview_update(lv, bl);

  ASSERT_PTR_NULL(lv->blasts);
  ASSERT_PTR_NULL(lv->latest_read);
  ck_assert_int_eq(conch_listview_has_unread_blasts(lv), false);

  conch_listview_free(lv);
}
END_TEST

START_TEST(test_listview_update_jumps_to_top_if_sticky) {
  blastlist *bl = blastlist_fixture_new(1);
  conch_cli_options opts = {.stick_to_top = true };
  listview *lv = conch_listview_new(&opts);
  conch_listview_update(lv, bl);

  blastdata b = {
    .id = 1,
  };
  resultset rs = {
    .count = 1, .blasts = &b,
  };
  conch_blastlist_prepend_resultset(bl, &rs);
  conch_listview_update(lv, bl);

  ck_assert_ptr_eq(lv->blasts->current, lv->blasts->head);
  ck_assert_ptr_eq(lv->latest_read, lv->blasts->head);
  ck_assert_int_eq(conch_listview_has_unread_blasts(lv), false);

  conch_listview_free(lv);
  conch_blastlist_free(bl);
}
END_TEST

START_TEST(test_listview_toggle_stick_to_top) {
  conch_cli_options opts = {.stick_to_top = true };
  listview *lv = conch_listview_new(&opts);

  ASSERT_PTR_NOT_NULL(lv);
  ck_assert_int_eq(lv->stick_to_top, true);

  conch_listview_toggle_stick_to_top(lv);
  ck_assert_int_eq(lv->stick_to_top, false);

  conch_listview_toggle_stick_to_top(lv);
  ck_assert_int_eq(lv->stick_to_top, true);
}
END_TEST

START_TEST(test_listview_cursor_movement) {
  blastlist *bl = blastlist_fixture_new(2);
  conch_cli_options opts = {.stick_to_top = false };
  listview *lv = conch_listview_new(&opts);
  conch_listview_update(lv, bl);

  // We can move forward
  conch_listview_select_next_blast(lv);
  ck_assert_ptr_eq(lv->blasts->tail, lv->blasts->current);

  // We can't move past the end
  conch_listview_select_next_blast(lv);
  ck_assert_ptr_eq(lv->blasts->tail, lv->blasts->current);

  // We can move back
  conch_listview_select_prev_blast(lv);
  ck_assert_ptr_eq(lv->blasts->head, lv->blasts->current);

  // We can't move past the beginning
  conch_listview_select_prev_blast(lv);
  ck_assert_ptr_eq(lv->blasts->head, lv->blasts->current);

  conch_listview_free(lv);
  conch_blastlist_free(bl);
}
END_TEST

START_TEST(test_listview_cursor_movement_scroll_down) {
  blastlist *bl = blastlist_fixture_new(3);
  conch_cli_options opts = {.stick_to_top = false };
  listview *lv = conch_listview_new(&opts);
  conch_listview_update(lv, bl);

  lv->bottom = lv->blasts->head->next;

  ck_assert_ptr_eq(lv->top, lv->blasts->head);
  ck_assert_ptr_eq(lv->top, lv->blasts->current);

  // We can move forward
  conch_listview_select_next_blast(lv);

  ck_assert_ptr_eq(lv->top, lv->blasts->current->prev);
  ck_assert_ptr_eq(lv->blasts->current, lv->blasts->head->next);

  // Moving past bottom should move the whole window down
  conch_listview_select_next_blast(lv);

  ck_assert_ptr_eq(lv->top, lv->blasts->current->prev);
  ck_assert_ptr_eq(lv->blasts->current, lv->blasts->head->next->next);
  ck_assert_ptr_eq(lv->bottom, lv->blasts->tail);

  conch_listview_free(lv);
  conch_blastlist_free(bl);
}
END_TEST

START_TEST(test_listview_cursor_movement_scroll_up) {
  blastlist *bl = blastlist_fixture_new(3);
  conch_cli_options opts = {.stick_to_top = false };
  listview *lv = conch_listview_new(&opts);
  conch_listview_update(lv, bl);

  lv->top = lv->blasts->head->next;
  lv->bottom = lv->blasts->tail;
  lv->blasts->current = lv->blasts->tail;

  // We can move up
  conch_listview_select_prev_blast(lv);

  ck_assert_ptr_eq(lv->top, lv->blasts->head->next);
  ck_assert_ptr_eq(lv->blasts->current, lv->blasts->head->next);

  // Moving past top should push the whole window up
  conch_listview_select_prev_blast(lv);

  ck_assert_ptr_eq(lv->top, lv->blasts->head);
  ck_assert_ptr_eq(lv->top, lv->blasts->current);
  ck_assert_ptr_eq(lv->bottom, lv->blasts->head->next);

  conch_listview_free(lv);
  conch_blastlist_free(bl);
}
END_TEST

START_TEST(test_listview_cursor_movement_updates_latest_read) {
  blastlist *bl = blastlist_fixture_new(3);
  conch_cli_options opts = {.stick_to_top = false };
  listview *lv = conch_listview_new(&opts);
  conch_listview_update(lv, bl);

  // 2 read, now add 2 more unread
  blastdata b2 = {
    .id = 2,
  };
  blastdata b3 = {
    .id = 3,
  };
  resultset rs = {
    .count = 2, .blasts = (blastdata[]){ b3, b2 },
  };

  blast *orighead = bl->head;

  conch_blastlist_prepend_resultset(bl, &rs);
  conch_listview_update(lv, bl);

  ck_assert_ptr_eq(lv->latest_read, orighead);
  ck_assert_int_eq(conch_listview_has_unread_blasts(lv), true);

  // Moving through already-read blasts doesn't change latest_read
  conch_listview_select_next_blast(lv);
  ck_assert_ptr_eq(lv->latest_read, orighead);
  ck_assert_int_eq(conch_listview_has_unread_blasts(lv), true);
  conch_listview_select_next_blast(lv);
  ck_assert_ptr_eq(lv->latest_read, orighead);
  ck_assert_int_eq(conch_listview_has_unread_blasts(lv), true);
  conch_listview_select_prev_blast(lv);
  ck_assert_ptr_eq(lv->latest_read, orighead);
  ck_assert_int_eq(conch_listview_has_unread_blasts(lv), true);
  conch_listview_select_prev_blast(lv);
  ck_assert_ptr_eq(lv->latest_read, orighead);
  ck_assert_int_eq(conch_listview_has_unread_blasts(lv), true);

  // Moving through unread blasts changes latest_read to current blast
  conch_listview_select_prev_blast(lv);
  ck_assert_ptr_eq(lv->latest_read, orighead->prev);
  ck_assert_int_eq(conch_listview_has_unread_blasts(lv), true);
  conch_listview_select_prev_blast(lv);
  ck_assert_ptr_eq(lv->latest_read, bl->head);
  ck_assert_int_eq(conch_listview_has_unread_blasts(lv), false);

  // Don't set latest_read off the top
  conch_listview_select_prev_blast(lv);
  ck_assert_ptr_eq(lv->latest_read, bl->head);
  ck_assert_int_eq(conch_listview_has_unread_blasts(lv), false);

  conch_listview_free(lv);
  conch_blastlist_free(bl);
}
END_TEST

START_TEST(test_listview_jump_to_top) {
  blastlist *bl = blastlist_fixture_new(3);
  conch_cli_options opts = {.stick_to_top = false };
  listview *lv = conch_listview_new(&opts);
  conch_listview_update(lv, bl);

  conch_listview_select_next_blast(lv);
  conch_listview_select_next_blast(lv);

  ck_assert_ptr_eq(bl->tail, bl->current);
  conch_listview_jump_to_top(lv);
  ck_assert_ptr_eq(bl->head, bl->current);

  conch_listview_free(lv);
  conch_blastlist_free(bl);
}
END_TEST

START_TEST(test_listview_jump_to_top_updates_latest_read) {
  blastlist *bl = blastlist_fixture_new(1);
  conch_cli_options opts = {.stick_to_top = false };
  listview *lv = conch_listview_new(&opts);
  conch_listview_update(lv, bl);

  ck_assert_ptr_eq(lv->latest_read, bl->head);

  // Don't update latest_read on update when stick_to_top disabled
  blastdata b = {
    .id = 1,
  };
  resultset rs = {
    .count = 1, .blasts = &b,
  };
  conch_blastlist_prepend_resultset(bl, &rs);
  conch_listview_update(lv, bl);

  ck_assert_ptr_eq(lv->latest_read, bl->tail);
  ck_assert_int_eq(conch_listview_has_unread_blasts(lv), true);

  // Update latest_read when jumping to top
  conch_listview_jump_to_top(lv);
  ck_assert_ptr_eq(lv->latest_read, bl->head);
  ck_assert_int_eq(conch_listview_has_unread_blasts(lv), false);

  conch_listview_free(lv);
  conch_blastlist_free(bl);
}
END_TEST

START_TEST(test_listview_has_unread_blasts) {
  conch_cli_options opts = {.stick_to_top = false };
  listview *lv = conch_listview_new(&opts);

  ck_assert_int_eq(conch_listview_has_unread_blasts(lv), false);

  blastlist *bl = blastlist_fixture_new(1);
  conch_listview_update(lv, bl);
  ck_assert_int_eq(conch_listview_has_unread_blasts(lv), false);

  // Indicate unread blasts when updating
  blastdata b = {
    .id = 1,
  };
  resultset rs = {
    .count = 1, .blasts = &b,
  };
  conch_blastlist_prepend_resultset(bl, &rs);
  conch_listview_update(lv, bl);
  ck_assert_int_eq(conch_listview_has_unread_blasts(lv), true);

  conch_listview_free(lv);
  conch_blastlist_free(bl);
}
END_TEST

START_TEST(test_listview_jump_to_next_unread) {
  blastlist *bl = blastlist_fixture_new(1);
  conch_cli_options opts = {.stick_to_top = false };
  listview *lv = conch_listview_new(&opts);

  conch_listview_jump_to_next_unread(lv);
  conch_listview_update(lv, bl);

  blastdata b2 = {
    .id = 2,
  };
  blastdata b3 = {
    .id = 3,
  };
  resultset rs = {
    .count = 2, .blasts = (blastdata[]){ b3, b2 },
  };
  conch_blastlist_prepend_resultset(bl, &rs);
  conch_listview_update(lv, bl);

  conch_listview_jump_to_next_unread(lv);
  ck_assert_ptr_eq(lv->latest_read, bl->tail->prev);
  ck_assert_int_eq(conch_listview_has_unread_blasts(lv), true);

  conch_listview_jump_to_next_unread(lv);
  ck_assert_ptr_eq(lv->latest_read, bl->head);
  ck_assert_int_eq(conch_listview_has_unread_blasts(lv), false);

  conch_listview_free(lv);
  conch_blastlist_free(bl);
}
END_TEST

Suite *listview_suite(void) {
  Suite *s = suite_create("listview");

  ADD_TEST_CASE(s, test_listview_new);
  ADD_TEST_CASE(s, test_listview_new_with_stick_to_top);
  ADD_TEST_CASE(s, test_listview_update_null_blastlist);
  ADD_TEST_CASE(s, test_listview_toggle_stick_to_top);
  ADD_TEST_CASE(s, test_listview_cursor_movement);
  ADD_TEST_CASE(s, test_listview_cursor_movement_updates_latest_read);
  ADD_TEST_CASE(s, test_listview_jump_to_top);
  ADD_TEST_CASE(s, test_listview_update_jumps_to_top_if_sticky);
  ADD_TEST_CASE(s, test_listview_jump_to_top_updates_latest_read);
  ADD_TEST_CASE(s, test_listview_has_unread_blasts);
  ADD_TEST_CASE(s, test_listview_jump_to_next_unread);
  ADD_TEST_CASE(s, test_listview_cursor_movement_scroll_down);
  ADD_TEST_CASE(s, test_listview_cursor_movement_scroll_up);
  return s;
}

CONCH_CHECK_MAIN(listview_suite)
