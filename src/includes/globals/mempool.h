#ifndef MEMPOOL_H
#define MEMPOOL_H

#include "constants.h"
#include "uthash.h"
#include "base_tx.h"

typedef struct MemPool {
    char id[TX_HASH_LEN];
    Transaction *tx;
} MemPool;

MemPool *mempool;

#endif
