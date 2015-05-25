#include "checkrunner.h"

#include "blastlist.h"
#include "conchbackend.h"

#define ASSERT_PTR_NULL(ptr) ck_assert_ptr_eq(ptr, NULL)
#define ASSERT_PTR_NOT_NULL(ptr) ck_assert_ptr_ne(ptr, NULL)

START_TEST(test_blastlist_null_resultset) {
  result_set *rs = NULL;
  blastlist *bl = conch_blastlist_new(rs);

  ASSERT_PTR_NULL(bl);

  conch_blastlist_free(bl);
}
END_TEST

START_TEST(test_blastlist_empty) {
  result_set rs = { 0 };
  blastlist *bl = conch_blastlist_new(&rs);

  ASSERT_PTR_NULL(bl);

  conch_blastlist_free(bl);
}
END_TEST

START_TEST(test_blastlist_single) {
  blast b = {
    .id = 1, .user = "giraffe", .content = "Mmm. Tasty leaves.",
  };
  result_set rs = {
    .count = 1, .blasts = &b,
  };
  blastlist *bl = conch_blastlist_new(&rs);

  // List should exist and have head
  ASSERT_PTR_NOT_NULL(bl);

  // List head should contain the correct content
  ck_assert(bl->id == b.id);
  ck_assert_str_eq(bl->user, b.user);
  ck_assert_str_eq(bl->content, b.content);

  // And both list pointers should be NULL
  ASSERT_PTR_NULL(bl->prev);
  ASSERT_PTR_NULL(bl->next);

  conch_blastlist_free(bl);
}
END_TEST

START_TEST(test_blastlist_multiple) {
  blast b1 = {
    .id = 1, .user = "giraffe", .content = "Mmm. Tasty leaves.",
  };
  blast b2 = {
    .id = 2, .user = "elephant", .content = "Splashy splashy water.",
  };
  blast b3 = {
    .id = 3, .user = "hippo", .content = "Mud, glorious mud!",
  };
  result_set rs = {
    .count = 3, .blasts = (blast[]){ b1, b2, b3 },
  };
  blastlist *bl = conch_blastlist_new(&rs);

  // List should exist
  ASSERT_PTR_NOT_NULL(bl);

  // Check list items
  blastlist *prev = NULL;
  blastlist *cur = bl;

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
  blast b1 = {
    .id = 1, .user = "giraffe", .content = "Mmm. Tasty leaves.",
  };
  result_set rs1 = {
    .count = 1, .blasts = &b1,
  };
  blastlist *rhs = conch_blastlist_new(&rs1);

  blastlist *bl = conch_blastlist_join(NULL, rhs);

  ck_assert_ptr_eq(bl, rhs);
}
END_TEST

START_TEST(test_blastlist_join_lhs_only) {
  blast b1 = {
    .id = 1, .user = "giraffe", .content = "Mmm. Tasty leaves.",
  };
  result_set rs1 = {
    .count = 1, .blasts = &b1,
  };
  blastlist *lhs = conch_blastlist_new(&rs1);

  blastlist *bl = conch_blastlist_join(lhs, NULL);

  ck_assert_ptr_eq(bl, lhs);
}
END_TEST

START_TEST(test_blastlist_join) {
  blast b1 = {
    .id = 1, .user = "giraffe", .content = "Mmm. Tasty leaves.",
  };
  blast b2 = {
    .id = 2, .user = "elephant", .content = "Splashy splashy water.",
  };
  result_set rs1 = {
    .count = 1, .blasts = &b1,
  };
  result_set rs2 = {
    .count = 1, .blasts = &b2,
  };
  blastlist *lhs = conch_blastlist_new(&rs1);
  blastlist *rhs = conch_blastlist_new(&rs2);

  blastlist *bl = conch_blastlist_join(lhs, rhs);

  ck_assert_ptr_eq(bl, lhs);
  ck_assert_ptr_eq(bl->next, rhs);
  ck_assert_ptr_eq(bl->next->prev, lhs);
}
END_TEST

Suite *blastlist_suite(void) {
  Suite *s = suite_create("blastlist");

  add_test_case(s, "null", test_blastlist_null_resultset);
  add_test_case(s, "empty", test_blastlist_empty);
  add_test_case(s, "single", test_blastlist_single);
  add_test_case(s, "multiple", test_blastlist_multiple);
  add_test_case(s, "join_null", test_blastlist_join_null);
  add_test_case(s, "join_rhs_only", test_blastlist_join_rhs_only);
  add_test_case(s, "join_lhs_only", test_blastlist_join_lhs_only);
  add_test_case(s, "join", test_blastlist_join);

  return s;
}

CONCH_CHECK_MAIN(blastlist_suite)
