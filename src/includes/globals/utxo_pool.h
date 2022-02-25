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
  UTXO utxo;
  UT_hash_handle hh;
} UTXOPool;

UTXOPool *utxo_pool;