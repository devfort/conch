#include "checkrunner.h"

#include "backend.h"
#include "blastlist.h"

#define ASSERT_PTR_NULL(ptr) ck_assert_ptr_eq(ptr, NULL)
#define ASSERT_PTR_NOT_NULL(ptr) ck_assert_ptr_ne(ptr, NULL)

START_TEST(test_blastlist_new) {
  blastlist *bl = conch_blastlist_new();

  ASSERT_PTR_NOT_NULL(bl);
  ASSERT_PTR_NULL(bl->head);
  ASSERT_PTR_NULL(bl->current);

  conch_blastlist_free(bl);
}
END_TEST

START_TEST(test_blastlist_from_resultset_null) {
  resultset *rs = NULL;
  blastlist *bl = conch_blastlist_from_resultset(rs);

  ASSERT_PTR_NOT_NULL(bl);
  ASSERT_PTR_NULL(bl->head);
  ASSERT_PTR_NULL(bl->current);

  conch_blastlist_free(bl);
}
END_TEST

START_TEST(test_blastlist_from_resultset_empty) {
  resultset rs = { 0 };
  blastlist *bl = conch_blastlist_from_resultset(&rs);

  ASSERT_PTR_NOT_NULL(bl);
  ASSERT_PTR_NULL(bl->head);
  ASSERT_PTR_NULL(bl->current);

  conch_blastlist_free(bl);
}
END_TEST

START_TEST(test_blastlist_from_resultset_single) {
  blastdata b = {
    .id = 1, .user = "giraffe", .content = "Mmm. Tasty leaves.",
  };
  resultset rs = {
    .count = 1, .blasts = &b,
  };
  blastlist *bl = conch_blastlist_from_resultset(&rs);

  // List should exist and have head
  ASSERT_PTR_NOT_NULL(bl);
  ASSERT_PTR_NOT_NULL(bl->head);

  // Current should be head
  ck_assert_ptr_eq(bl->head, bl->current);

  // Head should contain the correct content
  ck_assert(bl->head->id == b.id);
  ck_assert_str_eq(bl->head->user, b.user);
  ck_assert_str_eq(bl->head->content, b.content);

  // And both list pointers should be NULL
  ASSERT_PTR_NULL(bl->head->prev);
  ASSERT_PTR_NULL(bl->head->next);

  conch_blastlist_free(bl);
}
END_TEST

START_TEST(test_blastlist_from_resultset_multiple) {
  blastdata b1 = {
    .id = 1, .user = "giraffe", .content = "Mmm. Tasty leaves.",
  };
  blastdata b2 = {
    .id = 2, .user = "elephant", .content = "Splashy splashy water.",
  };
  blastdata b3 = {
    .id = 3, .user = "hippo", .content = "Mud, glorious mud!",
  };
  resultset rs = {
    .count = 3, .blasts = (blastdata[]){ b1, b2, b3 },
  };
  blastlist *bl = conch_blastlist_from_resultset(&rs);

  // List should exist
  ASSERT_PTR_NOT_NULL(bl);

  // List should exist and have head
  ASSERT_PTR_NOT_NULL(bl);
  ASSERT_PTR_NOT_NULL(bl->head);

  // Current should be head
  ck_assert_ptr_eq(bl->head, bl->current);

  blast *prev = NULL;
  blast *cur = bl->head;

  ck_assert(cur->id == b1.id);
  ck_assert_str_eq(cur->user, b1.user);
  ck_assert_str_eq(cur->content, b1.content);
  ck_assert_ptr_eq(cur->prev, prev);
  ASSERT_PTR_NOT_NULL(cur->next);

  prev = cur;
  cur = cur->next;

  ck_assert(cur->id == b2.id);
  ck_assert_str_eq(cur->user, b2.user);
  ck_assert_str_eq(cur->content, b2.content);
  ck_assert_ptr_eq(cur->prev, prev);
  ASSERT_PTR_NOT_NULL(cur->next);

  prev = cur;
  cur = cur->next;

  ck_assert(cur->id == b3.id);
  ck_assert_str_eq(cur->user, b3.user);
  ck_assert_str_eq(cur->content, b3.content);
  ck_assert_ptr_eq(cur->prev, prev);
  ASSERT_PTR_NULL(cur->next);

  conch_blastlist_free(bl);
}
END_TEST

START_TEST(test_blastlist_join_null) {
  blastlist *bl = conch_blastlist_join(NULL, NULL);

  ASSERT_PTR_NULL(bl);
}
END_TEST

START_TEST(test_blastlist_join_rhs_only) {
  blastdata b1 = {
    .id = 1, .user = "giraffe", .content = "Mmm. Tasty leaves.",
  };
  resultset rs1 = {
    .count = 1, .blasts = &b1,
  };
  blastlist *rhs = conch_blastlist_from_resultset(&rs1);

  blastlist *bl = conch_blastlist_join(NULL, rhs);

  ck_assert_ptr_eq(bl, rhs);
}
END_TEST

START_TEST(test_blastlist_join_lhs_only) {
  blastdata b1 = {
    .id = 1, .user = "giraffe", .content = "Mmm. Tasty leaves.",
  };
  resultset rs1 = {
    .count = 1, .blasts = &b1,
  };
  blastlist *lhs = conch_blastlist_from_resultset(&rs1);

  blastlist *bl = conch_blastlist_join(lhs, NULL);

  ck_assert_ptr_eq(bl, lhs);
}
END_TEST

START_TEST(test_blastlist_join) {
  blastdata b1 = {
    .id = 1, .user = "giraffe", .content = "Mmm. Tasty leaves.",
  };
  blastdata b2 = {
    .id = 2, .user = "elephant", .content = "Splashy splashy water.",
  };
  resultset rs1 = {
    .count = 1, .blasts = &b1,
  };
  resultset rs2 = {
    .count = 1, .blasts = &b2,
  };
  blastlist *lhs = conch_blastlist_from_resultset(&rs1);
  blastlist *rhs = conch_blastlist_from_resultset(&rs2);

  blastlist *bl = conch_blastlist_join(lhs, rhs);

  ck_assert_ptr_eq(bl->head, lhs->head);
  ck_assert_ptr_eq(bl->head->next, rhs->head);
  ck_assert_ptr_eq(bl->head->next->prev, lhs->head);
}
END_TEST

Suite *blastlist_suite(void) {
  Suite *s = suite_create("blastlist");

  ADD_TEST_CASE(s, test_blastlist_new);
  ADD_TEST_CASE(s, test_blastlist_from_resultset_null);
  ADD_TEST_CASE(s, test_blastlist_from_resultset_empty);
  ADD_TEST_CASE(s, test_blastlist_from_resultset_single);
  ADD_TEST_CASE(s, test_blastlist_from_resultset_multiple);
  ADD_TEST_CASE(s, test_blastlist_join_null);
  ADD_TEST_CASE(s, test_blastlist_join_rhs_only);
  ADD_TEST_CASE(s, test_blastlist_join_lhs_only);
  ADD_TEST_CASE(s, test_blastlist_join);

  return s;
}

CONCH_CHECK_MAIN(blastlist_suite)
