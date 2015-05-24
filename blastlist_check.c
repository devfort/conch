#include "checkrunner.h"

#include "blastlist.h"
#include "conchbackend.h"

START_TEST(test_blastlist_empty) {
  result_set rs = { 0 };
  blastlist *bl = conch_blastlist_new(&rs);

  ck_assert_ptr_ne(bl, (void *)0);
  ck_assert_ptr_eq(bl->head, (void *)0);

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
  ck_assert_ptr_ne(bl, (void *)0);
  ck_assert_ptr_ne(bl->head, (void *)0);

  // List head should contain the correct content
  ck_assert(bl->head->id == b.id);
  ck_assert_str_eq(bl->head->user, b.user);
  ck_assert_str_eq(bl->head->content, b.content);

  // And both LL pointers should be NULL
  ck_assert_ptr_eq(bl->head->prev, (void *)0);
  ck_assert_ptr_eq(bl->head->next, (void *)0);

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

  // List should exist and have head
  ck_assert_ptr_ne(bl, (void *)0);
  ck_assert_ptr_ne(bl->head, (void *)0);

  // Check list items
  blastlist_item *prev = NULL;
  blastlist_item *cur = bl->head;

  ck_assert(cur->id == b1.id);
  ck_assert_str_eq(cur->user, b1.user);
  ck_assert_str_eq(cur->content, b1.content);
  ck_assert_ptr_eq(cur->prev, prev);
  ck_assert_ptr_ne(cur->next, (void *)0);

  prev = cur;
  cur = cur->next;

  ck_assert(cur->id == b2.id);
  ck_assert_str_eq(cur->user, b2.user);
  ck_assert_str_eq(cur->content, b2.content);
  ck_assert_ptr_eq(cur->prev, prev);
  ck_assert_ptr_ne(cur->next, (void *)0);

  prev = cur;
  cur = cur->next;

  ck_assert(cur->id == b3.id);
  ck_assert_str_eq(cur->user, b3.user);
  ck_assert_str_eq(cur->content, b3.content);
  ck_assert_ptr_eq(cur->prev, prev);
  ck_assert_ptr_eq(cur->next, (void *)0);

  conch_blastlist_free(bl);
}
END_TEST

START_TEST(test_blastlist_insert) {
  blastlist_item *cur, *prev;
  blast b1 = {
    .id = 1, .user = "giraffe", .content = "Mmm. Tasty leaves.",
  };
  blast b2 = {
    .id = 2, .user = "elephant", .content = "Splashy splashy water.",
  };
  result_set rs = { 0 };
  result_set rs1 = {
    .count = 1, .blasts = &b1,
  };
  result_set rs2 = {
    .count = 1, .blasts = &b2,
  };

  // Construct an empty blastlist
  blastlist *bl = conch_blastlist_new(&rs);

  // Insert result set 1
  conch_blastlist_insert(bl, &rs1);

  cur = bl->head;

  // There should now be one item in the list
  ck_assert_ptr_ne(cur, (void *)0);
  ck_assert_str_eq(cur->user, b1.user);
  ck_assert_str_eq(cur->content, b1.content);
  ck_assert_ptr_eq(cur->prev, (void *)0);
  ck_assert_ptr_eq(cur->next, (void *)0);

  // Insert result set 2
  conch_blastlist_insert(bl, &rs2);

  prev = NULL;
  cur = bl->head;

  ck_assert_ptr_ne(cur, (void *)0);
  ck_assert(cur->id == b2.id);
  ck_assert_str_eq(cur->user, b2.user);
  ck_assert_str_eq(cur->content, b2.content);
  ck_assert_ptr_eq(cur->prev, prev);
  ck_assert_ptr_ne(cur->next, (void *)0);

  prev = cur;
  cur = cur->next;

  ck_assert(cur->id == b1.id);
  ck_assert_str_eq(cur->user, b1.user);
  ck_assert_str_eq(cur->content, b1.content);
  ck_assert_ptr_eq(cur->prev, prev);
  ck_assert_ptr_eq(cur->next, (void *)0);

  conch_blastlist_free(bl);
}
END_TEST

Suite *blastlist_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("blastlist");
  tc_core = tcase_create("create");
  tcase_add_test(tc_core, test_blastlist_empty);
  tcase_add_test(tc_core, test_blastlist_single);
  tcase_add_test(tc_core, test_blastlist_multiple);
  tcase_add_test(tc_core, test_blastlist_insert);
  suite_add_tcase(s, tc_core);

  return s;
}

CONCH_CHECK_MAIN(blastlist_suite)
