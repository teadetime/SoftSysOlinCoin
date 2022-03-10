#include "wallet.h"
#include "sign_tx.h"
#include "constants.h"

#include "mbedtls/ecp.h"

mbedtls_ecdsa_context **build_inputs(Transaction *tx, TxOptions *options) {
  size_t i, num_entries;
  mbedtls_ecdsa_context **keys;
  WalletPool *map_value, *end_value;

  options->out_total = options->tx_fee;
  for (i = 0; i < options->num_dests; i++)
    options->out_total += options->dests[i].amt;

  // Ensure we have enough in wallet before pops
  num_entries = 0;
  options->in_total = 0;
  map_value = wallet_pool;
  while (map_value != NULL && options->in_total < options->out_total) {
    if (map_value->entry->spent == 0) {
      num_entries++;
      options->in_total += map_value->entry->amt;
    }
    map_value = map_value->hh.next;
  }
  end_value = map_value;
  if (options->in_total < options->out_total) {
    printf("Not enough in wallet to build transaction!");
    exit(EXIT_FAILURE);
  }

  // Build the inputs
  tx->num_inputs = num_entries;
  tx->inputs = malloc(sizeof(Input) * num_entries);
  keys = malloc(sizeof(mbedtls_ecdsa_context*) * num_entries);

  i = 0;
  map_value = wallet_pool;
  while (map_value != end_value) {
    if (map_value->entry->spent)
      continue;

    tx->inputs[i].pub_key = malloc(sizeof(mbedtls_ecp_point));
    mbedtls_ecp_point_init(tx->inputs[i].pub_key);
    mbedtls_ecp_copy(
      tx->inputs[i].pub_key,
      &(map_value->entry->key_pair->MBEDTLS_PRIVATE(Q))
    );
    memset(tx->inputs[i].signature, 0, SIGNATURE_LEN);
    memcpy(tx->inputs[i].prev_tx_id, map_value->id.tx_hash, TX_HASH_LEN);
    tx->inputs[i].prev_utxo_output = map_value->id.vout;

    keys[i] = map_value->entry->key_pair;

    map_value->entry->spent = 1;
    map_value = map_value->hh.next;
    i++;
  }
  return keys;
}

mbedtls_ecdsa_context *build_outputs(Transaction *tx, TxOptions *options) {
  unsigned int num_outputs, self_output;
  unsigned long self_amt;
  mbedtls_ecdsa_context *key_pair;

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

  key_pair = NULL;
  if (self_output) {
    self_amt = options->in_total - options->out_total;
    key_pair = gen_keys();

    tx->outputs[options->num_dests].amt = self_amt;
    hash_pub_key(
      tx->outputs[options->num_dests].public_key_hash,
      key_pair
    );
  }
  return key_pair;
}

void sign_tx(Transaction *tx, mbedtls_ecdsa_context **keys) {
  unsigned char tx_hash[TX_HASH_LEN];

  hash_tx(tx_hash, tx);
  for (size_t i = 0; i < tx->num_inputs; i++) {
    tx->inputs[i].sig_len = write_sig(
      tx->inputs[i].signature, SIGNATURE_LEN,
      tx_hash, TX_HASH_LEN,
      keys[i]
    );
  }
}

Transaction *build_tx(TxOptions *options) {
  Transaction *tx;
  mbedtls_ecdsa_context **input_keys;
  mbedtls_ecdsa_context *new_key;

  tx = malloc(sizeof(Transaction));
  input_keys = build_inputs(tx, options);
  new_key = build_outputs(tx, options);
  sign_tx(tx, input_keys);

  if (new_key != NULL)
    key_pool_add(new_key);

  free(input_keys);
  //TODO: Free options

  return tx;
}
