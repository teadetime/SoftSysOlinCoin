#include "queue.h"
#include <unistd.h>
#include "pthread.h"

void *add_and_delay(void *queue){
  Queue *thread_queue = (Queue *) queue;
  int *test1 = malloc(sizeof(int));
  *test1 = 5;
  printf("Starting thread 1\n");
  queue_add_int(thread_queue, test1);
  printf("Done Adding thread 1\n");

  printf("Add and Delay Returns ...\n");
  return NULL;
}

void *add(void *queue){
  Queue *thread_queue = (Queue *) queue;

  int *test1 = malloc(sizeof(int));
  *test1 = 90;
  printf("Starting thread 2\n");
  queue_add_int(thread_queue, test1);
  printf("Done Adding thread 2\n");
  printf("Add Returns ...\n");
  return NULL;
}

int main() {

  Queue *test_q = queue_init();

  int *test1 = malloc(sizeof(int));
  *test1 = 4;
  queue_add_int(test_q, test1);
  int *pop_int = queue_pop_int(test_q);
  printf("Popped val: %i\n", *pop_int);

  pthread_t thread1, thread2;
  int  iret1, iret2;
  /* Create independent threads each of which will execute function */

  iret1 = pthread_create( &thread1, NULL, add_and_delay, (void*) test_q);
  iret2 = pthread_create( &thread2, NULL, add, (void*) test_q);

  /* Wait till threads are complete before main continues. Unless we  */
  /* wait we run the risk of executing an exit which will terminate   */
  /* the process and all threads before the threads have completed.   */

  pthread_join( thread1, NULL);
  pthread_join( thread2, NULL); 

  printf("Thread 1 returns: %d\n",iret1);
  printf("Thread 2 returns: %d\n",iret2);
  return 0;
}
