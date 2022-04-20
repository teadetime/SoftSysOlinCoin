#include "wallet.h"
#include "constants.h"
#include "crypto.h"
#include "ser_wallet.h"

#include "mbedtls/ecp.h"

mbedtls_ecdsa_context **build_inputs(Transaction *tx, TxOptions *options) {
  size_t i, num_entries;
  mbedtls_ecdsa_context **keys;
  //WalletPool *map_value, *end_value;
  options->out_total = options->tx_fee;
  for (i = 0; i < options->num_dests; i++)
    options->out_total += options->dests[i].amt;

  // Ensure we have enough in wallet before pops
  num_entries = 0;
  options->in_total = 0;

  leveldb_readoptions_t *roptions = leveldb_readoptions_create();
  leveldb_iterator_t *iter = leveldb_create_iterator(wallet_pool_db, roptions);

  for (leveldb_iter_seek_to_first(iter); leveldb_iter_valid(iter); leveldb_iter_next(iter))
  {
    size_t key_len, value_len;
    unsigned const char *key_ptr = (unsigned const char*) leveldb_iter_key(iter, &key_len);
    unsigned const char *value_ptr = (unsigned const char*) leveldb_iter_value(iter, &value_len);

    WalletEntry *read_wallet_entry = deser_wallet_entry_alloc(NULL, (unsigned char*)value_ptr);
    if(read_wallet_entry->spent == 0){
      num_entries++;
      options->in_total += read_wallet_entry->amt;
    }
    free(read_wallet_entry);
    // free(key_ptr);
    // free(value_ptr);
    if(options->in_total >= options->out_total){
      break;
    }
  }
  leveldb_iter_destroy(iter);
  leveldb_readoptions_destroy(roptions);

  if (options->in_total < options->out_total) {
    printf("Not enough in wallet to build transaction!");
    exit(EXIT_FAILURE);
  }

  // Build the inputs
  tx->num_inputs = num_entries;
  tx->inputs = malloc(sizeof(Input) * num_entries);
  keys = malloc(sizeof(mbedtls_ecdsa_context*) * num_entries);

  unsigned int entries_to_get = num_entries;
  i = 0;
  leveldb_readoptions_t *roptions2 = leveldb_readoptions_create();
  leveldb_iterator_t *iter2 = leveldb_create_iterator(wallet_pool_db, roptions2);
  for (leveldb_iter_seek_to_first(iter2); leveldb_iter_valid(iter2); leveldb_iter_prev(iter2))
  {
    size_t key_len, value_len;
    unsigned const char *key_ptr = (unsigned const char*) leveldb_iter_key(iter2, &key_len);
    unsigned const char *value_ptr = (unsigned const char*) leveldb_iter_value(iter2, &value_len);

    WalletEntry *read_wallet_entry = deser_wallet_entry_alloc(NULL, (unsigned char*)value_ptr);
    if (read_wallet_entry->spent) {
      free(read_wallet_entry);
      continue;
    }
    

    tx->inputs[i].pub_key = malloc(sizeof(mbedtls_ecp_point));
    mbedtls_ecp_point_init(tx->inputs[i].pub_key);
    mbedtls_ecp_copy(
      tx->inputs[i].pub_key,
      &(read_wallet_entry->key_pair->MBEDTLS_PRIVATE(Q))
    );
    memset(tx->inputs[i].signature, 0, SIGNATURE_LEN);
    tx->inputs[i].sig_len = 0;
    memcpy(tx->inputs[i].prev_tx_id, key_ptr, TX_HASH_LEN);
    tx->inputs[i].prev_utxo_output = *(int *)(key_ptr+TX_HASH_LEN);

    //TODO THIS NEESS TO COPY SO IT CAN BE FREED
    unsigned char *key_cpy = ser_keypair_alloc(NULL, read_wallet_entry->key_pair);
    keys[i] = deser_keypair_alloc(NULL, key_cpy);
    // if(key_cpy != NULL){
    //   free(key_cpy);
    // }
    read_wallet_entry->spent = 1;

    wallet_pool_add_wallet_entry_leveldb((unsigned char *)key_ptr, read_wallet_entry);
    free(read_wallet_entry);
    entries_to_get--;
    i++;
    if(entries_to_get <= 0){
      break;
    }
  }
  leveldb_iter_destroy(iter2);
  leveldb_readoptions_destroy(roptions2);



  // i = 0;
  // map_value = wallet_pool;
  // while (map_value != end_value) {
  //   if (map_value->entry->spent)
  //     continue;

  //   tx->inputs[i].pub_key = malloc(sizeof(mbedtls_ecp_point));
  //   mbedtls_ecp_point_init(tx->inputs[i].pub_key);
  //   mbedtls_ecp_copy(
  //     tx->inputs[i].pub_key,
  //     &(map_value->entry->key_pair->MBEDTLS_PRIVATE(Q))
  //   );
  //   memset(tx->inputs[i].signature, 0, SIGNATURE_LEN);
  //   tx->inputs[i].sig_len = 0;
  //   memcpy(tx->inputs[i].prev_tx_id, map_value->id.tx_hash, TX_HASH_LEN);
  //   tx->inputs[i].prev_utxo_output = map_value->id.vout;

  //   keys[i] = map_value->entry->key_pair;

  //   map_value->entry->spent = 1;
  //   map_value = map_value->hh.next;
  //   i++;
  // }
  return keys;
}

mbedtls_ecdsa_context *build_outputs(Transaction *tx, TxOptions *options) {
  unsigned int self_output;
  unsigned long self_amt;
  mbedtls_ecdsa_context *key_pair;

  self_output = 0;
  if (options->in_total != options->out_total)
    self_output = 1;

  tx->num_outputs = options->num_dests + self_output;
  tx->outputs = malloc(sizeof(Output) * tx->num_outputs);

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
    key_pool_add_leveldb(new_key);

  free(input_keys);
  //TODO: Free options

  return tx;
}
