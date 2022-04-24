#include "queue.h"

int main() {

  Queue *test_q = queue_init();

  int *test1 = malloc(sizeof(int));
  *test1 = 4;
  queue_add_void(test_q, test1);
  int *pop_int = queue_pop_void(test_q);
  printf("Popped val: %i\n", *pop_int);



  return 0;
}
