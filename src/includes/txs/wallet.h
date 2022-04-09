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

/**
 * @brief Build the inputs for the given transaction.
 *
 * @param tx The transaction to add inputs to
 * @param options The options for building the transaction. Only num_dests,
 *  dests and tx_fee should be set. The in_total and out_total fields will be set
 *  by the function.
 * @return Array of key pairs, each corresponding to a new input in the
 *  transaction. return[0] should sign tx->inputs[0], and so on.
 */
mbedtls_ecdsa_context **build_inputs(Transaction *tx, TxOptions *options);

/**
 * @brief Builds the outputs for the given transaction.
 *
 * @param tx The transaction to add outputs to
 * @param options The options for building the transaction. All fields should be
 *  set. It is expected that build_inputs() was called before hand.
 * @return Single key pair that correspondes to the self-output. NULL if no
 *  self-output was needed.
 */
mbedtls_ecdsa_context *build_outputs(Transaction *tx, TxOptions *options);

/**
 * @brief Signs the given transaction
 *
 * @param tx The transaction to sign
 * @param keys Array of key pairs corresponding to the transaction inputs. It is
 *  expected that this was returned by build_inputs()
 */
void sign_tx(Transaction *tx, mbedtls_ecdsa_context **keys);

/**
 * @brief Builds a transaction
 *
 * Builds a transaction given options. Collects transactions from the wallet
 * pool such that the outputs and transaction fee can be fulfilled, then puts
 * together the transaction. Creates an output with a new key pair stored in the
 * key pool to handle "change"
 *
 * @param options The options for building the transaction. Only num_dests,
 *  dests and tx_fee should be set. dests is an array of outputs describing the
 *  outputs of the tx and tx_fee is the transaction fee to use. in_total and
 *  out_total should not be set.
 * @return A new transaction
 */
Transaction *build_tx(TxOptions *options);
