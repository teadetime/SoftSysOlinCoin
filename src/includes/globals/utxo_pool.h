#pragma once

#include "constants.h"
#include "uthash.h"
#include "base_tx.h"

typedef struct UTXOPoolKey {
  unsigned char tx_hash[TX_HASH_LEN];
  unsigned int vout;
} UTXOPoolKey;

typedef struct UTXOPool {
  UTXOPoolKey id;
  UTXO *utxo;
  UT_hash_handle hh;
} UTXOPool;

UTXOPool *utxo_pool;

void utxo_pool_init();
UTXO *utxo_pool_add(Transaction *tx, unsigned int vout);
UTXO *utxo_pool_remove(unsigned char *tx_hash, unsigned int vout);
UTXO *utxo_pool_remove_key(UTXOPoolKey *key);
UTXO *utxo_pool_find(unsigned char *tx_hash, unsigned int vout);
UTXOPool *utxo_pool_find_node(unsigned char *tx_hash, unsigned int vout);
UTXOPool *utxo_pool_find_node_key(UTXOPoolKey *key);
