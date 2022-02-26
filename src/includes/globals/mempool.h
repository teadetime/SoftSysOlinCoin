#pragma once

#include "constants.h"
#include "uthash.h"
#include "base_tx.h"

typedef struct MemPool {
  unsigned char id[TX_HASH_LEN];
  Transaction *tx;
  UT_hash_handle hh;
} MemPool;

MemPool *mempool;

void mempool_init();
Transaction *mempool_add(Transaction *tx);
Transaction *mempool_remove(unsigned char *tx_hash);
Transaction *mempool_find(unsigned char *tx_hash);
MemPool *mempool_find_node(unsigned char *tx_hash);
