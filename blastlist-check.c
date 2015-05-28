#include "checkrunner.h"

#include "backend.h"
#include "blastlist.h"

START_TEST(test_blastlist_new) {
  blastlist *bl = conch_blastlist_new();

  ASSERT_PTR_NOT_NULL(bl);
  ASSERT_PTR_NULL(bl->head);
  ASSERT_PTR_NULL(bl->current);
  ASSERT_PTR_NULL(bl->tail);

  conch_blastlist_free(bl);
}
END_TEST

START_TEST(test_blastlist_append_resultset) {
  blastdata b1 = {
    .id = 1,
    .user = "giraffe",
    .content = "Mmm. Tasty leaves.",
    .extended = "I'm some code",
  };
  blastdata b2 = {
    .id = 2, .user = "elephant", .content = "Splashy splashy water.",
  };
  blastdata b3 = {
    .id = 3, .user = "hippo", .content = "Mud, glorious mud!",
  };
  resultset *rsn = NULL;
  resultset rse = {.count = 0, .blasts = NULL };
  resultset rs3 = {
    .count = 1, .blasts = &b3,
  };
  resultset rs21 = {
    .count = 2, .blasts = (blastdata[]){ b2, b1 },
  };

  blastlist *bl = conch_blastlist_new();

  conch_blastlist_append_resultset(bl, rsn);

  ASSERT_PTR_NOT_NULL(bl);
  ASSERT_PTR_NULL(bl->head);
  ASSERT_PTR_NULL(bl->current);
  ASSERT_PTR_NULL(bl->tail);

  conch_blastlist_append_resultset(bl, &rse);

  ASSERT_PTR_NOT_NULL(bl);
  ASSERT_PTR_NULL(bl->head);
  ASSERT_PTR_NULL(bl->current);
  ASSERT_PTR_NULL(bl->tail);

  conch_blastlist_append_resultset(bl, &rs3);

  ASSERT_PTR_NOT_NULL(bl);
  ASSERT_PTR_NOT_NULL(bl->head);
  ck_assert_ptr_eq(bl->head, bl->current);
  ck_assert_ptr_eq(bl->head, bl->tail);

  conch_blastlist_append_resultset(bl, &rs21);

  ASSERT_PTR_NOT_NULL(bl);
  ASSERT_PTR_NOT_NULL(bl->head);
  ck_assert_ptr_eq(bl->head, bl->current);
  ck_assert_ptr_ne(bl->head, bl->tail);

  blast *prev = NULL;
  blast *cur = bl->head;

  ck_assert_int_eq(cur->id, 3);
  ck_assert_str_eq(cur->user, b3.user);
  ck_assert_str_eq(cur->content, b3.content);
  ASSERT_PTR_NULL(cur->extended);
  ck_assert_ptr_eq(cur->prev, prev);

  prev = cur;
  cur = cur->next;

  ck_assert_int_eq(cur->id, 2);
  ck_assert_str_eq(cur->user, b2.user);
  ck_assert_str_eq(cur->content, b2.content);
  ASSERT_PTR_NULL(cur->extended);
  ck_assert_ptr_eq(cur->prev, prev);

  prev = cur;
  cur = cur->next;

  ck_assert_int_eq(cur->id, 1);
  ck_assert_str_eq(cur->user, b1.user);
  ck_assert_str_eq(cur->content, b1.content);
  ASSERT_PTR_NOT_NULL(cur->extended);
  ck_assert_str_eq(cur->extended, b1.extended);
  ck_assert_ptr_eq(cur->prev, prev);

  conch_blastlist_free(bl);
}
END_TEST

START_TEST(test_blastlist_prepend_resultset) {
  blastdata b1 = {
    .id = 1, .user = "giraffe", .content = "Mmm. Tasty leaves.",
  };
  blastdata b2 = {
    .id = 2, .user = "elephant", .content = "Splashy splashy water.",
  };
  blastdata b3 = {
    .id = 3, .user = "hippo", .content = "Mud, glorious mud!",
  };
  resultset *rsn = NULL;
  resultset rse = {.count = 0, .blasts = NULL };
  resultset rs1 = {
    .count = 1, .blasts = &b1,
  };
  resultset rs32 = {
    .count = 2, .blasts = (blastdata[]){ b3, b2 },
  };

  blastlist *bl = conch_blastlist_new();

  conch_blastlist_prepend_resultset(bl, rsn);

  ASSERT_PTR_NOT_NULL(bl);
  ASSERT_PTR_NULL(bl->head);
  ASSERT_PTR_NULL(bl->current);
  ASSERT_PTR_NULL(bl->tail);

  conch_blastlist_prepend_resultset(bl, &rse);

  ASSERT_PTR_NOT_NULL(bl);
  ASSERT_PTR_NULL(bl->head);
  ASSERT_PTR_NULL(bl->current);
  ASSERT_PTR_NULL(bl->tail);

  conch_blastlist_prepend_resultset(bl, &rs1);

  ASSERT_PTR_NOT_NULL(bl);
  ASSERT_PTR_NOT_NULL(bl->head);
  ck_assert_ptr_eq(bl->head, bl->current);
  ck_assert_ptr_eq(bl->head, bl->tail);

  blast *prev;
  blast *cur = bl->current;
  conch_blastlist_prepend_resultset(bl, &rs32);

  ASSERT_PTR_NOT_NULL(bl);
  ASSERT_PTR_NOT_NULL(bl->head);
  ck_assert_ptr_ne(bl->head, bl->current);
  ck_assert_ptr_ne(bl->head, bl->tail);
  ck_assert_ptr_eq(bl->current, cur);
  ck_assert_ptr_eq(bl->current, bl->tail);

  prev = NULL;
  cur = bl->head;

  ck_assert_int_eq(cur->id, 3);
  ck_assert_str_eq(cur->user, b3.user);
  ck_assert_str_eq(cur->content, b3.content);
  ck_assert_ptr_eq(cur->prev, prev);

  prev = cur;
  cur = cur->next;

  ck_assert_int_eq(cur->id, 2);
  ck_assert_str_eq(cur->user, b2.user);
  ck_assert_str_eq(cur->content, b2.content);
  ck_assert_ptr_eq(cur->prev, prev);

  prev = cur;
  cur = cur->next;

  ck_assert_int_eq(cur->id, 1);
  ck_assert_str_eq(cur->user, b1.user);
  ck_assert_str_eq(cur->content, b1.content);
  ck_assert_ptr_eq(cur->prev, prev);

  conch_blastlist_free(bl);
}
END_TEST

Suite *blastlist_suite(void) {
  Suite *s = suite_create("blastlist");

  ADD_TEST_CASE(s, test_blastlist_new);
  ADD_TEST_CASE(s, test_blastlist_append_resultset);
  ADD_TEST_CASE(s, test_blastlist_prepend_resultset);

  return s;
}

CONCH_CHECK_MAIN(blastlist_suite)
