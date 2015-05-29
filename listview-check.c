#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "checkrunner.h"

#include "backend.h"

#include "blastlist.h"
#include "cli.h"
#include "listview.h"

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
  b->content = calloc(21, sizeof(blast));
  b->user = strdup("conch");

  int ret = snprintf(b->content, 21, "%" PRIid, b->id);
  assert(ret > 0);

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
  conch_cli_options opts = {.stick_to_head = false };
  listview *lv = conch_listview_new(&opts);

  ASSERT_PTR_NOT_NULL(lv);
  ASSERT_PTR_NULL(lv->blasts);
  ASSERT_PTR_NULL(lv->latest_read);
  ck_assert_int_eq(lv->stick_to_head, false);
  ck_assert_int_eq(conch_listview_has_unread_blasts(lv), false);

  conch_listview_free(lv);
}
END_TEST

START_TEST(test_listview_new_with_stick_to_head) {
  conch_cli_options opts = {.stick_to_head = true };
  listview *lv = conch_listview_new(&opts);

  ASSERT_PTR_NOT_NULL(lv);
  ck_assert_int_eq(lv->stick_to_head, true);

  conch_listview_free(lv);
}
END_TEST

START_TEST(test_listview_update_null_blastlist) {
  blastlist *bl = NULL;
  conch_cli_options opts = {.stick_to_head = false };
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
  conch_cli_options opts = {.stick_to_head = true };
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

START_TEST(test_listview_toggle_stick_to_head) {
  conch_cli_options opts = {.stick_to_head = true };
  listview *lv = conch_listview_new(&opts);

  ASSERT_PTR_NOT_NULL(lv);
  ck_assert_int_eq(lv->stick_to_head, true);

  conch_listview_toggle_stick_to_head(lv);
  ck_assert_int_eq(lv->stick_to_head, false);

  conch_listview_toggle_stick_to_head(lv);
  ck_assert_int_eq(lv->stick_to_head, true);
}
END_TEST

START_TEST(test_listview_cursor_movement) {
  blastlist *bl = blastlist_fixture_new(2);
  conch_cli_options opts = {.stick_to_head = false };
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
  conch_cli_options opts = {.stick_to_head = false };
  listview *lv = conch_listview_new(&opts);
  conch_listview_update(lv, bl);

  lv->bottom = lv->blasts->head->next;

  /*
   * head  0  top    current
   *       1  bottom
   * tail  2
   */
  ck_assert_ptr_eq(lv->top, lv->blasts->head);
  ck_assert_ptr_eq(lv->top, lv->blasts->current);

  conch_listview_select_next_blast(lv);

  /*
   * head  0  top
   *       1  bottom current
   * tail  2
   */
  ck_assert_ptr_eq(lv->top, lv->blasts->head);
  ck_assert_ptr_eq(lv->blasts->current, lv->blasts->head->next);
  ck_assert_ptr_eq(lv->blasts->current, lv->bottom);

  // Moving past bottom should push the whole window down, meaning that
  // both bottom and current should be adjusted.
  //
  // We don't adjust top when moving down because only the render loop can
  // compute which blast is actually at the top.
  conch_listview_select_next_blast(lv);

  /*
   * head  0  top
   *       1
   * tail  2  bottom current
   */
  ck_assert_ptr_eq(lv->blasts->current, lv->blasts->tail);
  ck_assert_ptr_eq(lv->bottom, lv->blasts->tail);

  conch_listview_free(lv);
  conch_blastlist_free(bl);
}
END_TEST

START_TEST(test_listview_cursor_movement_scroll_up) {
  blastlist *bl = blastlist_fixture_new(3);
  conch_cli_options opts = {.stick_to_head = false };
  listview *lv = conch_listview_new(&opts);
  conch_listview_update(lv, bl);

  lv->top = lv->blasts->head->next;
  lv->bottom = lv->blasts->tail;
  lv->blasts->current = lv->blasts->tail;

  /*
   * head  0
   *       1  top
   * tail  2  bottom current
   */

  conch_listview_select_prev_blast(lv);

  /*
   * head  0
   *       1  top current
   * tail  2  bottom
   */
  ck_assert_ptr_eq(lv->top, lv->blasts->head->next);
  ck_assert_ptr_eq(lv->blasts->current, lv->blasts->head->next);

  // Moving past top should push the whole window up, meaning that both top and
  // current should be adjusted.
  //
  // We don't adjust bottom when moving up because only the render loop can
  // compute which blast is actually at the bottom.
  conch_listview_select_prev_blast(lv);

  /*
   * head  0  top current
   *       1
   * tail  2  bottom current
   */
  ck_assert_ptr_eq(lv->top, lv->blasts->head);
  ck_assert_ptr_eq(lv->blasts->current, lv->blasts->head);

  conch_listview_free(lv);
  conch_blastlist_free(bl);
}
END_TEST

START_TEST(test_listview_render_direction_scroll_down) {
  blastlist *bl = blastlist_fixture_new(4);
  conch_cli_options opts = {.stick_to_head = false };
  listview *lv = conch_listview_new(&opts);
  conch_listview_update(lv, bl);

  lv->bottom = lv->blasts->head->next;

  /*
   * head  0  top    current
   *       1  bottom
   *       2
   * tail  3
   */
  ck_assert_int_eq(lv->render_from_bottom, false);

  conch_listview_select_next_blast(lv);

  /*
   * head  0  top
   *       1  bottom current
   *       2
   * tail  3
   */
  ck_assert_int_eq(lv->render_from_bottom, false);

  // Now move bottom down one, but also tell the listview that it was partially
  // rendered. Unlike the last case, this should trigger a change of render
  // direction.
  lv->bottom = lv->bottom->next;
  lv->render_overflow = true;
  conch_listview_select_next_blast(lv);

  /*
   * head  0  top
   *       1
   *       2  bottom current
   * tail  3
   */
  ck_assert_int_eq(lv->render_from_bottom, true);

  // Now, with render_overflow false, we should trigger a render direction
  // change because we actually try to move current *past* bottom.
  lv->render_from_bottom = false;
  lv->render_overflow = false;
  conch_listview_select_next_blast(lv);

  /*
   * head  0  top
   *       1
   *       2
   * tail  3  bottom current
   */
  ck_assert_int_eq(lv->render_from_bottom, true);

  conch_listview_free(lv);
  conch_blastlist_free(bl);
}
END_TEST

START_TEST(test_listview_render_direction_scroll_up) {
  blastlist *bl = blastlist_fixture_new(4);
  conch_cli_options opts = {.stick_to_head = false };
  listview *lv = conch_listview_new(&opts);
  conch_listview_update(lv, bl);

  lv->blasts->current = lv->blasts->tail;
  lv->bottom = lv->blasts->tail;
  lv->top = lv->blasts->tail->prev;
  lv->render_from_bottom = true;

  /*
   * head  0
   *       1
   *       2  top
   * tail  3  bottom current
   */
  ck_assert_int_eq(lv->render_from_bottom, true);

  conch_listview_select_prev_blast(lv);

  /*
   * head  0
   *       1
   *       2 top current
   * tail  3 bottom
   */
  ck_assert_int_eq(lv->render_from_bottom, true);

  // Now move top up one, but also tell the listview that it was partially
  // rendered. Unlike the last case, this should trigger a change of render
  // direction.
  lv->top = lv->top->prev;
  lv->render_overflow = true;
  conch_listview_select_prev_blast(lv);

  /*
   * head  0
   *       1  top current
   *       2
   * tail  3  bottom
   */
  ck_assert_int_eq(lv->render_from_bottom, false);

  // Now, with render_overflow false, we should trigger a render direction
  // change because we actually try to move current *past* bottom.
  lv->render_from_bottom = true;
  lv->render_overflow = false;
  conch_listview_select_prev_blast(lv);

  /*
   * head  0  top current
   *       1
   *       2
   * tail  3  bottom
   */
  ck_assert_int_eq(lv->render_from_bottom, false);

  conch_listview_free(lv);
  conch_blastlist_free(bl);
}
END_TEST

START_TEST(test_listview_cursor_movement_updates_latest_read) {
  blastlist *bl = blastlist_fixture_new(3);
  conch_cli_options opts = {.stick_to_head = false };
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
  conch_cli_options opts = {.stick_to_head = false };
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
  conch_cli_options opts = {.stick_to_head = false };
  listview *lv = conch_listview_new(&opts);
  conch_listview_update(lv, bl);

  ck_assert_ptr_eq(lv->latest_read, bl->head);

  // Don't update latest_read on update when stick_to_head disabled
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
  conch_cli_options opts = {.stick_to_head = false };
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
  conch_cli_options opts = {.stick_to_head = false };
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

START_TEST(test_listview_search_forward) {
  blastlist *bl = blastlist_fixture_new(3);

  conch_cli_options opts = {.stick_to_head = false };
  listview *lv = conch_listview_new(&opts);
  conch_listview_update(lv, bl);

  lv->bottom = lv->blasts->head->next;

  blast *found = conch_listview_find_and_select_blast(lv, "1");
  ck_assert_ptr_eq(found, lv->blasts->current);
  ck_assert_ptr_eq(found, lv->blasts->head->next);

  found = conch_listview_find_and_select_blast(lv, "2");
  ck_assert_ptr_eq(found, lv->blasts->current);
  ck_assert_ptr_eq(found, lv->blasts->tail);
  ck_assert_ptr_eq(found, lv->top);

  conch_listview_free(lv);
  conch_blastlist_free(bl);
}
END_TEST

Suite *listview_suite(void) {
  Suite *s = suite_create("listview");

  ADD_TEST_CASE(s, test_listview_new);
  ADD_TEST_CASE(s, test_listview_new_with_stick_to_head);
  ADD_TEST_CASE(s, test_listview_update_null_blastlist);
  ADD_TEST_CASE(s, test_listview_toggle_stick_to_head);
  ADD_TEST_CASE(s, test_listview_cursor_movement);
  ADD_TEST_CASE(s, test_listview_cursor_movement_updates_latest_read);
  ADD_TEST_CASE(s, test_listview_jump_to_top);
  ADD_TEST_CASE(s, test_listview_update_jumps_to_top_if_sticky);
  ADD_TEST_CASE(s, test_listview_jump_to_top_updates_latest_read);
  ADD_TEST_CASE(s, test_listview_has_unread_blasts);
  ADD_TEST_CASE(s, test_listview_jump_to_next_unread);
  ADD_TEST_CASE(s, test_listview_cursor_movement_scroll_down);
  ADD_TEST_CASE(s, test_listview_cursor_movement_scroll_up);
  ADD_TEST_CASE(s, test_listview_render_direction_scroll_down);
  ADD_TEST_CASE(s, test_listview_render_direction_scroll_up);
  ADD_TEST_CASE(s, test_listview_search_forward);
  return s;
}

CONCH_CHECK_MAIN(listview_suite)
