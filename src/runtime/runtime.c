#include "queue.h"
#include <unistd.h>
#include "pthread.h"
#include "validate_block.h"
#include "handle_block.h"
#include "create_block.h"

#include "init_globals.h"
#include "init_db.h"

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

void *node_block_thread(void *arg){
  Globals *globals = arg;
  while(1){
    printf("Node Thread Waiting to pop Block\n");
    Block *popped_block = queue_pop_void(globals->queue_block);
    printf("Node Thread Block Popped\n");
    //Now aquire locks for validation (Utxo pool and blockcahin)
    printf("Node Thread Waiting on lock for validation\n");
    pthread_mutex_lock(&globals->utxo_pool_lock);
    pthread_mutex_lock(&globals->blockchain_lock);
    int block_valid = validate_block(popped_block);


    if(block_valid == 0){
      //Now aquire additional locks for handling
      printf("Node Thread Waiting on lock for Handling\n");
      pthread_mutex_lock(&globals->utxo_to_tx_lock);
      pthread_mutex_lock(&globals->wallet_pool_lock);
      pthread_mutex_lock(&globals->key_pool_lock);
      pthread_mutex_lock(&globals->mempool_lock);
      accept_block(popped_block);
      free_block(popped_block);
      pthread_mutex_unlock(&globals->utxo_to_tx_lock);
      pthread_mutex_unlock(&globals->wallet_pool_lock);
      pthread_mutex_unlock(&globals->key_pool_lock);
      pthread_mutex_unlock(&globals->mempool_lock);
      printf("Node Thread unlockedr Handling\n");
    }
    pthread_mutex_unlock(&globals->utxo_pool_lock);
    pthread_mutex_unlock(&globals->blockchain_lock);
  }
  return NULL;
}

void *miner_thread(void *arg){
  Globals *globals = arg;
  while(1){
    printf("Miner Thread waiting on lock to create Block\n");
    //Now aquire locks for creating a block!
    pthread_mutex_lock(&globals->utxo_pool_lock);
    pthread_mutex_lock(&globals->blockchain_lock);
    pthread_mutex_lock(&globals->key_pool_lock) ;
    pthread_mutex_lock(&globals->mempool_lock);
    printf("Miner Thread got lock to create Block\n");
    Block *new_block = create_block_alloc();

    pthread_mutex_unlock(&globals->utxo_pool_lock);
    pthread_mutex_unlock(&globals->blockchain_lock);
    pthread_mutex_unlock(&globals->key_pool_lock) ;
    pthread_mutex_unlock(&globals->mempool_lock);
    printf("Miner Thread done creating Block\n");

    while(try_header_hash(&(new_block->header)) != 0){
      change_nonce(new_block);
    }

    printf("Miner mined a block Block\n");
    queue_add_void(globals->queue_block, new_block);
  }
  return NULL;
}

int main() {

  Globals *globals = init_globals();

  // Intitialize the globabls!
  node_init(PROD_DB_LOC);

  pthread_t node_block, node_tx, shell, miner;
  int node_block_ret, node_tx_ret, shell_ret, miner_ret;
  /* Create independent threads each of which will execute function */

  // node_block_ret = pthread_create( &node_block, NULL, pop, (void*) globals->queue_block);
  // sleep(1);
  // node_tx_ret = pthread_create( &node_tx, NULL, add_and_delay, (void*) globals->queue_block);
  
  node_block_ret = pthread_create( &node_block, NULL, node_block_thread, (void*) globals);
  sleep(1);
  miner_ret = pthread_create( &miner, NULL, miner_thread, (void*) globals);
  
  /* Wait till threads are complete before main continues. Unless we  */
  /* wait we run the risk of executing an exit which will terminate   */
  /* the process and all threads before the threads have completed.   */

  pthread_join(node_block, NULL);
  pthread_join(miner, NULL); 

  printf("Thread 1 returns: %d\n",node_block_ret);
  printf("Thread 2 returns: %d\n", miner_ret);
  return 0;
}
