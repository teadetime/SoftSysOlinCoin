#ifndef UTXO_POOL_H
#define UTXO_POOL_H

#include "constants.h"
#include "uthash.h"
#include "base_tx.h"

typedef struct UTXOPoolKey {
    char tx_hash[TX_HASH_LEN];
    unsigned int vout;
} UTXOPoolKey;

typedef struct UTXOPool {
    UTXOPoolKey id;
    UTXO utxo;
    UT_hash_handle hh;
} UTXOPool;

UTXOPool *utxo_pool;

#endif
