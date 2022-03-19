#pragma once

#include "base_tx.h"
#include "constants.h"
#include "wallet_pool.h"

#include "mbedtls/ecdsa.h"

typedef struct {
  size_t num_dests;
  Output *dests;
  unsigned long tx_fee;
  unsigned long in_total;  // Used internally
  unsigned long out_total;  // Used internally
} TxOptions;

mbedtls_ecdsa_context **build_inputs(Transaction *tx, TxOptions *options);
mbedtls_ecdsa_context *build_outputs(Transaction *tx, TxOptions *options);
void sign_tx(Transaction *tx, mbedtls_ecdsa_context **keys);
Transaction *build_tx(TxOptions *options);
