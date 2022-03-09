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
  size_t num_dests;
  Output *dests;
  unsigned long tx_fee;
  unsigned long in_total;
  unsigned long out_total;
} TxOptions;

void wallet_pool_init();
void wallet_pool_append(WalletEntry *entry);
WalletEntry *wallet_pool_pop();

WalletEntry *build_wallet_entry(Transaction *tx, unsigned int vout, mbedtls_ecdsa_context *key_pair);
void free_wallet_entry(WalletEntry *entry);

mbedtls_ecdsa_context **build_inputs(Transaction *tx, TxOptions *options);
WalletEntry *build_outputs(Transaction *tx, TxOptions *options);
void sign_tx(Transaction *tx, mbedtls_ecdsa_context **keys);
Transaction *build_tx(TxOptions *options);

WalletPool verified_wallet_pool;
