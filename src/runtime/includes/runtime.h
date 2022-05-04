#pragma once
#include "queue.h"
#include "pthread.h"
#include "semaphore.h"
#include <mqueue.h>

#define MAX_INCOMING_CONNECTIONS 10
#define INCOMING_QUEUE "/client_incoming"
#define OUTGOING_QUEUE_FORMAT "/server_outgoing_%d"

#define QUEUE_PERMISSIONS 0660
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10

#define BLOCK_ID 1
#define TX_ID 0

typedef struct {
  Queue *queue_block;
  Queue *queue_tx;
  pthread_mutex_t utxo_pool_lock;
  pthread_mutex_t utxo_to_tx_lock;
  pthread_mutex_t blockchain_lock;
  pthread_mutex_t wallet_pool_lock;
  pthread_mutex_t key_pool_lock;
  pthread_mutex_t mempool_lock;
  int *miner_update;
  pthread_mutex_t miner_update_lock;
  int connected;
  char **q_server_individual; // send the same thing out to multiple nodes
  char *q_client; // Only one queue from all the incoming blocks
} Globals;
