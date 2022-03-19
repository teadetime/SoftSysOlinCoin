#include <stdio.h>
#include "minunit.h"
#include "list.h"

int tests_run = 0;

static char *test_list_build() {
  List *list;

  list = build_list(sizeof(long));

  mu_assert(
      "List was not initialized",
      list->data != NULL
  );
  mu_assert(
      "List length incorrect",
      list->len == 0
  );
  mu_assert(
      "List total length incorrect",
      list->total_len == 1
  );
  mu_assert(
      "List element size incorrect",
      list->element_size == sizeof(long)
  );

  return NULL;
}

static char *test_list_append() {
  List *list;
  long entry;

  list = build_list(sizeof(long));
  entry = 42;

  list_append(list, &entry);

  mu_assert(
      "List entry not appended",
      **(long**)list->data == entry
  );
  mu_assert(
      "List length post append incorrect",
      list->len == 1
  );

  return NULL;
}

static char *test_list_pop() {
  List *list;
  long entry, *ret;

  list = build_list(sizeof(long));
  entry = 42;

  list_append(list, &entry);
  ret = list_pop(list);

  mu_assert(
      "List entry address wrong",
      ret == &entry
  );
  mu_assert(
      "List entry value wrong",
      *ret == entry
  );
  mu_assert(
      "List length post pop incorrect",
      list->len == 0
  );

  return NULL;
}

static char *all_tests() {
  mu_run_test(test_list_build);
  mu_run_test(test_list_append);
  mu_run_test(test_list_pop);
  return NULL;
}

int main() {
  char *result = all_tests();
  if (result != NULL) {
    printf("%s\n", result);
  } else {
    printf("list.c passing!\n");
  }
  printf("Tests run: %d\n", tests_run);

  return result != 0;
}
