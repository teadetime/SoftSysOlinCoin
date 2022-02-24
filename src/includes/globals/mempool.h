#ifndef MEMPOOL_H
#define MEMPOOL_H

#include "constants.h"
#include "uthash.h"
#include "base_tx.h"

typedef struct MemPool {
    char id[TX_HASH_LEN];
    Transaction *tx;
    UT_hash_handle hh;
} MemPool;

MemPool *mempool;

void mempool_init();
Transaction* mempool_add(Transaction *tx);
Transaction* mempool_remove(Transaction *tx);
MemPool* mempool_find(char *tx_hash);

#endif
