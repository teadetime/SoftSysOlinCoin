#include "queue.h"
#include <unistd.h>
#include "pthread.h"
#include "validate_block.h"
#include "handle_block.h"
#include "validate_tx.h"
#include "handle_tx.h"
#include "create_block.h"
#include "shell.h"
#include "runtime.h"
#include "init_globals.h"
#include "init_db.h"
#include "server.h"
#include "client.h"

#define MAX_INCOMING_CONNECTIONS 10

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
  if (pthread_mutex_init(&new_globals->miner_update_lock, NULL) != 0) {
    mutex_fail = 1;
  }
  if (mutex_fail == 1){
    fprintf(stderr,"mutex init has failed\n");
    return NULL;
  }
  new_globals->miner_update = malloc(sizeof(int));
  *new_globals->miner_update = 1;

  new_globals->q_server_individual = malloc(MAX_INCOMING_CONNECTIONS * sizeof(char *));
  new_globals->connected = 0;
  new_globals->q_client = malloc(32);
  strcpy(new_globals->q_client, "/client_incoming");
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
      pthread_mutex_lock(&globals->miner_update_lock);
      *globals->miner_update = 0;
      pthread_mutex_unlock(&globals->miner_update_lock);
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

void *node_tx_thread(void *arg){
  Globals *globals = arg;
  while(1){
    //printf("NodeTX Thread Waiting to pop TX\n");
    Transaction *popped_tx = queue_pop_void(globals->queue_tx);
    //printf("NodeTX Thread TX Popped\n");
    //Now aquire locks for validation (Utxo pool and blockcahin)
    //printf("NodeTX Thread Waiting on lock for validation\n");


    pthread_mutex_lock(&globals->utxo_pool_lock);
    pthread_mutex_lock(&globals->utxo_to_tx_lock);
    pthread_mutex_lock(&globals->mempool_lock);

    int tx_valid = validate_tx_incoming(popped_tx);


    if(tx_valid == 0){
      //Now aquire additional locks for handling
      handle_tx(popped_tx);
      free_tx(popped_tx);
    }
    pthread_mutex_unlock(&globals->mempool_lock);
    pthread_mutex_unlock(&globals->utxo_to_tx_lock);
    pthread_mutex_unlock(&globals->utxo_pool_lock);
    //printf("NodeTX Thread unlocked from validation\n");
  }
  return NULL;
}


void add_to_outgoing(Globals *globals){
  printf("FOund a block and adding outgoing data");
  for(int i = 0; i < globals->connected; i++){
    char *q_name = globals->q_server_individual[i];
    mqd_t child_mq;
    if ((child_mq = mq_open (q_name, O_WRONLY)) == -1) {
        perror ("Server: mq_open (server)");
        exit (1);
    }
    // send message to server
    char *test = "fake_block";
    if (mq_send (child_mq, test, strlen (test) + 1, 1) == -1) {
        perror ("Client: Not able to send message to server");
    }
    if (mq_close (child_mq) == -1) {
      perror ("Client: mq_close");
      exit (1);
    }


  }
}

void *miner_thread(void *arg){
  Globals *globals = arg;
  unsigned long hash_check_flag = 10000;
  int new_block_in_chain = 1; //1 is false 0 is true
  while(1){
    //printf("Miner Thread waiting on lock to create Block\n");
    //Now aquire locks for creating a block!
    pthread_mutex_lock(&globals->utxo_pool_lock);
    pthread_mutex_lock(&globals->blockchain_lock);
    pthread_mutex_lock(&globals->key_pool_lock) ;
    pthread_mutex_lock(&globals->mempool_lock);
    //printf("Miner Thread got lock to create Block\n");
    Block *new_block = create_block_alloc();

    pthread_mutex_unlock(&globals->utxo_pool_lock);
    pthread_mutex_unlock(&globals->blockchain_lock);
    pthread_mutex_unlock(&globals->key_pool_lock) ;
    pthread_mutex_unlock(&globals->mempool_lock);
    //printf("Miner Thread done creating Block\n");

    while(try_header_hash(&(new_block->header)) != 0){
      change_nonce(new_block);
      if(new_block->header.nonce % hash_check_flag == 0){
        pthread_mutex_lock(&globals->miner_update_lock);
        if(globals->miner_update == 0){
          new_block_in_chain = 0;
          *globals->miner_update = 1;
        }
        pthread_mutex_unlock(&globals->miner_update_lock);
        if(new_block_in_chain == 0){
          break;
        }
      }
    }
    if(new_block_in_chain == 0){
      free_block(new_block);
      continue;
      new_block_in_chain = 1;
    }
    print_block(new_block,"");
    printf("Miner mined a block Block\n");
    queue_add_void(globals->queue_block, new_block);
    // Serialize block and send it to our peers
    add_to_outgoing(globals);
  }
  return NULL;
}


void *shell_thread(void *arg){
  Globals *globals = arg;
  shell_init();
  shell_loop(globals);
  return NULL;
}

int main() {

  Globals *globals = init_globals();

  // Intitialize the globabls!
  node_init(PROD_DB_LOC);

  pthread_t node_block, node_tx, shell, miner, server, client;
  int node_block_ret, node_tx_ret, shell_ret, miner_ret, server_ret, client_ret;

  /* Create independent threads each of which will execute function */
  server_ret = pthread_create( &server, NULL, server_thread, (void*) globals);
  sleep(5);
  // Now Create the Client Thread
  client_ret = pthread_create( &client, NULL, client_thread, (void*) globals);

  node_block_ret = pthread_create( &node_block, NULL, node_block_thread, (void*) globals);
  node_tx_ret = pthread_create( &node_tx, NULL, node_tx_thread, (void*) globals);
  shell_ret = pthread_create( &shell, NULL, shell_thread, (void*) globals);
  miner_ret = pthread_create( &miner, NULL, miner_thread, (void*) globals);
  
  /* Wait till threads are complete before main continues. Unless we  */
  /* wait we run the risk of executing an exit which will terminate   */
  /* the process and all threads before the threads have completed.   */

  pthread_join(shell, NULL);
  pthread_join(server, NULL);
  pthread_join(node_block, NULL);
  pthread_join(node_tx, NULL);
  pthread_join(miner, NULL); 

  printf("Node Block returns: %d\n",node_block_ret);
  printf("Node TX returns: %d\n", node_tx_ret);
  printf("Shell returns: %d\n", shell_ret);
  printf("Miner returns: %d\n", miner_ret);
  printf("Server returns: %d\n", server_ret);
  return 0; 
}
