#include "queue.h"
#include <unistd.h>
#include "pthread.h"

typedef struct Globals {
  Queue *queue_block;
  Queue *queue_tx;
  pthread_mutex_t utxo_pool_lock;
  pthread_mutex_t utxo_to_tx_lock;
  pthread_mutex_t blockchain_lock;
  pthread_mutex_t wallet_pool_lock;
  pthread_mutex_t key_pool_lock;
  pthread_mutex_t mempool_lock;
} Globals;

Globals *init_globals(){
  Globals *new_globals = malloc(sizeof(Globals));
  new_globals->queue_block = queue_init();
  new_globals->queue_tx = queue_init();
  int mutex_fail = 0;
  if (pthread_mutex_init(&new_globals->utxo_pool_lock, NULL) != 0) {
    mutex_fail = 1;
  }
  if (pthread_mutex_init(&new_globals->utxo_to_tx_lock, NULL) != 0) {
    mutex_fail = 1;
  }
  if (pthread_mutex_init(&new_globals->blockchain_lock, NULL) != 0) {
    mutex_fail = 1;
  }
  if (pthread_mutex_init(&new_globals->wallet_pool_lock, NULL) != 0) {
    mutex_fail = 1;
  }
  if (pthread_mutex_init(&new_globals->key_pool_lock, NULL) != 0) {
    mutex_fail = 1;
  }
  if (pthread_mutex_init(&new_globals->mempool_lock, NULL) != 0) {
    mutex_fail = 1;
  }
  if (mutex_fail == 1){
    fprintf(stderr,"mutex init has failed\n");
    return NULL;
  }
  return new_globals;
}

void *add_and_delay(void *queue){
  Queue *thread_queue = (Queue *) queue;
  int *test1 = malloc(sizeof(int));
  *test1 = 5;
  printf("Starting thread 1\n");
  queue_add_void(thread_queue, test1);
  printf("Done Adding thread 1\n");

  printf("Add and Delay Returns ...\n");
  return NULL;
}

void *pop(void *queue){
  Queue *thread_queue = (Queue *) queue;

  printf("Starting thread pop\n");
  int *ret = queue_pop_void(thread_queue);
  printf("Done pop thread 2, popped: %i\n", *ret);
  printf("Add Returns ...\n");
  return NULL;
}

int main() {

  Globals *globals = init_globals();

  int *test1 = malloc(sizeof(int));
  *test1 = 4;
  queue_add_void(globals->queue_block, test1);
  int *pop_int = queue_pop_void(globals->queue_block);
  printf("Popped val: %i\n", *pop_int);

  pthread_t node_block, node_tx, shell, miner;
  int node_block_ret, node_tx_ret, shell_ret, miner_ret;
  /* Create independent threads each of which will execute function */

  node_block_ret = pthread_create( &node_block, NULL, pop, (void*) globals->queue_block);
  sleep(1);
  node_tx_ret = pthread_create( &node_tx, NULL, add_and_delay, (void*) globals->queue_block);
  

  /* Wait till threads are complete before main continues. Unless we  */
  /* wait we run the risk of executing an exit which will terminate   */
  /* the process and all threads before the threads have completed.   */

  pthread_join( node_block, NULL);
  pthread_join( node_tx, NULL); 

  printf("Thread 1 returns: %d\n",node_block_ret);
  printf("Thread 2 returns: %d\n", node_tx_ret);
  return 0;
}
