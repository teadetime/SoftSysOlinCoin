#include "wallet.h"
#include "sign_tx.h"
#include "constants.h"

#include "mbedtls/ecp.h"

/* POOL FUNCTIONS */

void wallet_pool_init() {
  verified_wallet_pool.data = malloc(sizeof(WalletEntry*));
  verified_wallet_pool.len = 0;
  verified_wallet_pool.total_len = 1;
}

void wallet_pool_append(WalletEntry *entry) {
  WalletEntry **new_data;

  if (verified_wallet_pool.len >= verified_wallet_pool.total_len) {
    // Make new array with double the size, then copy data in
    new_data = malloc(sizeof(WalletEntry*) * verified_wallet_pool.total_len * 2);
    memcpy(
      new_data, verified_wallet_pool.data,
      verified_wallet_pool.len * sizeof(WalletEntry)
    );

    free(verified_wallet_pool.data);
    verified_wallet_pool.data = new_data;
    verified_wallet_pool.total_len = verified_wallet_pool.total_len * 2;
  }
  verified_wallet_pool.data[verified_wallet_pool.len++] = entry;
}

WalletEntry *wallet_pool_pop() {
  if (verified_wallet_pool.len == 0)
    return NULL;
  return verified_wallet_pool.data[--(verified_wallet_pool.len)];
}

WalletEntry *build_wallet_entry(
    Transaction *tx, unsigned int vout, mbedtls_ecdsa_context *key_pair
) {
  WalletEntry *new_entry;

  new_entry = malloc(sizeof(WalletEntry));
  hash_tx(new_entry->id.tx_hash, tx);
  new_entry->id.vout = vout;
  new_entry->amt = tx->outputs[vout].amt;
  new_entry->key_pair = key_pair;  // Key not copied!

  return new_entry;
}

void free_wallet_entry(WalletEntry *entry) {
  if (entry == NULL)
    return;
  mbedtls_ecdsa_free(entry->key_pair);
  free(entry);
}

/* BUILD FUNCTIONS */

mbedtls_ecdsa_context **build_inputs(Transaction *tx, TxOptions *options) {
  size_t i, num_entries;
  WalletEntry *curr_entry;
  mbedtls_ecdsa_context **keys;

  options->out_total = options->tx_fee;
  for (i = 0; i < options->num_dests; i++)
    options->out_total += options->dests[i].amt;

  // Ensure we have enough in wallet before pops
  i = verified_wallet_pool.len;
  options->in_total = 0;
  while (--i >= 0 && options->in_total < options->out_total)
    options->in_total += verified_wallet_pool.data[i]->amt;
  if (options->in_total < options->out_total) {
    printf("Not enough in wallet to build transaction!");
    exit(EXIT_FAILURE);
  }

  // Build the inputs
  // TODO: This will not work for a finished codebase. By popping and
  // immediatly removing the entry from the pool, we assume the transaction
  // being built is guarenteed to be validated and mined.
  num_entries = verified_wallet_pool.len - (i + 1);
  tx->num_inputs = num_entries;
  tx->inputs = malloc(sizeof(Input) * num_entries);
  keys = malloc(sizeof(mbedtls_ecdsa_context*) * num_entries);
  for (i = 0; i < num_entries; i++) {
    curr_entry = wallet_pool_pop();

    mbedtls_ecp_copy(tx->inputs[i].pub_key, &(curr_entry->key_pair->MBEDTLS_PRIVATE(Q)));
    memset(tx->inputs[i].signature, 0, SIGNATURE_LEN);
    memcpy(tx->inputs[i].prev_tx_id, curr_entry->id.tx_hash, TX_HASH_LEN);
    tx->inputs[i].prev_utxo_output = curr_entry->id.vout;

    keys[i] = curr_entry->key_pair;
    free(curr_entry);
  }
  return keys;
}

WalletEntry *build_outputs(Transaction *tx, TxOptions *options) {
  unsigned int num_outputs, self_output, self_amt;
  mbedtls_ecp_keypair *key_pair;
  WalletEntry *new_entry;

  self_output = 0;
  if (options->in_total != options->out_total)
    self_output = 1;
  num_outputs = options->num_dests + self_output;

  tx->num_outputs = num_outputs;
  tx->outputs = malloc(sizeof(Output) * num_outputs);

  for (size_t i = 0; i < options->num_dests; i++) {
    tx->outputs[i].amt = options->dests[i].amt;
    memcpy(
      tx->outputs[i].public_key_hash,
      options->dests[i].public_key_hash,
      PUB_KEY_HASH_LEN
    );
  }

  new_entry = NULL;
  if (self_output) {
    self_amt = options->in_total - options->out_total;
    key_pair = gen_keys();
    new_entry = malloc(sizeof(WalletEntry));

    tx->outputs[options->num_dests].amt = self_amt;
    hash_pub_key(
      tx->outputs[options->num_dests].public_key_hash,
      key_pair
    );

    // Note: new_entry->id.tx_hash IS NOT SET
    // It needs to be calculated with the whole Transaction
    new_entry->amt = self_amt;
    new_entry->key_pair = key_pair;
    new_entry->id.vout = options->num_dests;
  }
  return new_entry;
}

void sign_tx(Transaction *tx, mbedtls_ecdsa_context **keys) {
  unsigned char tx_hash[TX_HASH_LEN];
  hash_tx(tx_hash, tx);
  for (size_t i = 0; i < tx->num_inputs; i++) {
    write_sig(
      tx->inputs[i].signature, SIGNATURE_LEN,
      tx_hash, TX_HASH_LEN,
      keys[i]
    );
  }
}

Transaction *build_tx(TxOptions *options) {
  Transaction *tx;
  mbedtls_ecp_keypair **input_keys;
  WalletEntry *new_entry;

  tx = malloc(sizeof(Transaction));
  input_keys = build_inputs(tx, options);
  new_entry = build_outputs(tx, options);
  sign_tx(tx, input_keys);

  if (new_entry != NULL) {
    hash_tx(new_entry->id.tx_hash, tx);
    wallet_pool_append(new_entry);
  }

  return tx;
}
