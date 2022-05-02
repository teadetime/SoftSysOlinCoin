#pragma once
#include "queue.h"
#include "pthread.h"
#include "semaphore.h"
#include <mqueue.h>

typedef struct Globals {
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
  mqd_t q_server; // Main queu that gets distributed to all the individual queues
  char *q_client; // Only one queue from all the incoming blocks
} Globals;