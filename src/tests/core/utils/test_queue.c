#include "queue.h"

int main() {

  Queue *test_q = queue_init();

  int *test1 = malloc(sizeof(int));
  *test1 = 4;
  int add_ret = queue_add_int(test_q, test1);
  int *pop_int = queue_pop_int(test_q);
  printf("Popped val: %i", *pop_int);
  return 0;
}
