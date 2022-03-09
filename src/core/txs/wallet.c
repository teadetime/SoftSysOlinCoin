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

void build_wallet_entry(
    Transaction *tx, unsigned int vout, mbedtls_ecdsa_context *key_pair
) {
  WalletEntry *new_entry;

  new_entry = malloc(sizeof(WalletEntry));
  hash_tx(new_entry->id.tx_hash, tx);
  new_entry->id.vout = vout;
  new_entry->amt = tx->outputs[vout].amt;
  new_entry->key_pair = key_pair;  // Struct not copied!

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
  unsigned long total_amt, collected_amt;
  size_t i, num_entries;
  WalletEntry *curr_entry;
  mbedtls_ecdsa_context **keys;

  total_amt = 0;
  for (i = 0; i < options->num_dests; i++)
    total_amt += options->dests[i].amt;

  // Ensure we have enough in wallet before pops
  i = verified_wallet_pool.len;
  collected_amt = 0;
  while (--i >= 0 && collected_amt < total_amt)
    collected_amt += verified_wallet_pool.data[i]->amt;
  if (collected_amt < total_amt) {
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

void build_outputs(TxBuilder *builder) {
  Output *curr_output;
  mbedtls_ecdsa_context *curr_key_pair;
  unsigned char ser_key[PUB_KEY_SER_LEN];
  size_t num_bytes;

  builder->tx->num_outputs = builder->num_outputs;
  builder->tx->outputs = malloc(sizeof(Output) * builder->num_outputs);
  builder->keys = malloc(sizeof(mbedtls_ecdsa_context*) * builder->num_outputs);

  for (size_t i = 0; i < builder->num_outputs; i++) {
    curr_key_pair = gen_keys();
    builder->keys[i] = curr_key_pair;

    curr_output = builder->tx->outputs + i;
    curr_output->amt = builder->amts[i];

    // Serialize then hash pubkey
    mbedtls_ecp_point_write_binary(
      &curr_key_pair->MBEDTLS_PRIVATE(grp), &curr_key_pair->MBEDTLS_PRIVATE(Q),
      MBEDTLS_ECP_PF_COMPRESSED, &num_bytes, ser_key, PUB_KEY_SER_LEN
    );
    hash_sha256(curr_output->public_key_hash, ser_key, num_bytes);
  }
}

void sign_tx(TxBuilder *builder) {
  Input *curr_input;
  Transaction *curr_tx;
  mbedtls_ecdsa_context *key_pair;
  unsigned char tx_hash[TX_HASH_LEN];
  size_t num_bytes;

  curr_tx = builder->tx;
  hash_tx(tx_hash, curr_tx);

  for (size_t i = 0; i < curr_tx->num_inputs; i++) {
    curr_input = curr_tx->inputs + i;
    key_pair = builder->keys[i];

    mbedtls_ecdsa_write_signature(
      key_pair, MBEDTLS_MD_SHA256,
      tx_hash, TX_HASH_LEN,
      curr_input->signature, SIGNATURE_LEN, &num_bytes,
      mbedtls_ctr_drbg_random, ctr_drbg
    );
  }
}

void build_tx(TxBuilder *builder) {
  builder->tx = malloc(sizeof(Transaction));
  build_inputs(builder);
  build_outputs(builder);
  sign_tx(builder);
}
