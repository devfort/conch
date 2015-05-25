#include <stdio.h>
#include "checkrunner.h"

#include "wordwrap.h"

#define ASSERT_PTR_NULL(ptr) ck_assert_ptr_eq(ptr, NULL)
#define ASSERT_PTR_NOT_NULL(ptr) ck_assert_ptr_ne(ptr, NULL)
#define ASSERT_STRNCMP_EQUAL(a, b, n) ck_assert(strncmp(a, b, n) == 0)

START_TEST(test_wordwrap) {
  wordwrap_s wrap;
  init_wordwrap(&wrap, "We're going to need a bigger boat", 13);

  token_s expected[] = { {.y = 0, .x = 0, .length = 5, .word = "We're" },
                         {.y = 0, .x = 6, .length = 5, .word = "going" },
                         {.y = 1, .x = 0, .length = 2, .word = "to" },

                         {.y = 1, .x = 3, .length = 4, .word = "need" },
                         {.y = 1, .x = 8, .length = 1, .word = "a" },

                         {.y = 2, .x = 0, .length = 6, .word = "bigger" },
                         {.y = 2, .x = 7, .length = 4, .word = "boat" } };

  for (int i = 0; i < sizeof(expected) / sizeof(token_s); ++i) {
    token_s *word = wordwrap(&wrap);
    token_s *expect = &expected[i];

    ASSERT_PTR_NOT_NULL(word);
    ASSERT_PTR_NOT_NULL(expect);
    ck_assert_int_eq(expect->y, word->y);
    ck_assert_int_eq(expect->x, word->x);
    ck_assert_uint_eq(expect->length, word->length);
    ASSERT_STRNCMP_EQUAL(word->word, expect->word, expect->length);
  }

  token_s *null_word = wordwrap(&wrap);
  ASSERT_PTR_NULL(null_word);
}
END_TEST

Suite *wordwrap_suite(void) {
  Suite *s = suite_create("wordwrap");

  ADD_TEST_CASE(s, test_wordwrap);

  return s;
}

CONCH_CHECK_MAIN(wordwrap_suite)
