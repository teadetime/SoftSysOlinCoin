#include <stdio.h>

#include <setjmp.h>
#include <cmocka.h>

#include "list.h"

static void test_list_build(void **state) {
  List *list;

  (void)state;

  list = build_list(sizeof(long));
  assert_ptr_not_equal(list->data, NULL);
  assert_int_equal(list->len, 0);
  assert_int_equal(list->total_len, 1);
  assert_int_equal(list->element_size, sizeof(long));
}

static void test_list_append(void **state) {
  List *list;
  long entry;

  (void)state;

  list = build_list(sizeof(long));
  entry = 42;
  list_append(list, &entry);
  assert_int_equal(*(long*)(list->data[0]), entry);
  assert_int_equal(list->len, 1);
}

static void test_list_pop(void **state) {
  List *list;
  long entry, *ret;

  (void)state;

  list = build_list(sizeof(long));
  entry = 42;

  list_append(list, &entry);
  ret = list_pop(list);

  assert_ptr_equal(ret, &entry);
  assert_int_equal(*ret, entry);
  assert_int_equal(list->len, 0);
}

int main() {
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_list_build),
    cmocka_unit_test(test_list_append),
    cmocka_unit_test(test_list_pop),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
