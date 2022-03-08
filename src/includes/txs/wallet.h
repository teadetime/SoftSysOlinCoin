#pragma once

#include "base_tx.h"
#include "constants.h"
#include "utxo_pool.h"

#include "mbedtls/ecdsa.h"

typedef struct {
  UTXOPoolKey id;
  mbedtls_ecdsa_context *key_pair;
  unsigned long amt;
} WalletEntry;

typedef struct {
  WalletEntry **data;
  size_t len;
  size_t total_len;
} WalletPool;

typedef struct {
  size_t num_outputs;
  unsigned long *amts;
  Transaction *tx;
  mbedtls_ecdsa_context **keys;
} TxBuilder;

void wallet_pool_init();
void wallet_pool_append(WalletEntry *entry);
void wallet_pool_update(TxBuilder *builder);
WalletEntry *wallet_pool_pop();

void build_inputs(TxBuilder *builder);
void build_outputs(TxBuilder *builder);
void sign_tx(TxBuilder *builder);
void build_tx(TxBuilder *builder);

WalletPool verified_wallet_pool;
