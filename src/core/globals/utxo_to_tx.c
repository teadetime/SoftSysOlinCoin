#include <stdio.h>
#include "utxo_to_tx.h"

void utxo_to_tx_init() {
  utxo_to_tx = NULL;
}

int utxo_to_tx_add(unsigned char *utxo_tx_hash, unsigned int vout, unsigned char *tx_hash) {
  UTXOToTx *new_entry, *found_entry;

  new_entry = malloc(sizeof(UTXOToTx));

  memset(&(new_entry->id), 0, sizeof(UTXOPoolKey));
  memcpy(new_entry->id.tx_hash, utxo_tx_hash, TX_HASH_LEN);
  new_entry->id.vout = vout;
  memcpy(new_entry->tx_hash, tx_hash, TX_HASH_LEN);

  found_entry = utxo_to_tx_find_node_key(&(new_entry->id));
  if (found_entry == NULL) {
    HASH_ADD(hh, utxo_to_tx, id, sizeof(UTXOPoolKey), new_entry);
    return 0;
  } else {
    free(new_entry);
    return 1;
  }
}

int utxo_to_tx_add_tx(Transaction *tx) {
  int ret_val;
  unsigned char tx_hash[TX_HASH_LEN];

  if (tx->num_inputs == 0)
    return 0;

  hash_tx(tx_hash, tx);
  for (size_t i = 0; i < tx->num_inputs; i++) {
    ret_val = utxo_to_tx_add(
      tx->inputs[i].prev_tx_id,
      tx->inputs[i].prev_utxo_output,
      tx_hash
    );
    if (ret_val != 0) {
      // Hard exit here since throwing a recoverable error would mean going
      // through and removing everything we've added up until this point. Would
      // be nicer, but unclear what we should do if removing then fails.
      printf("Duplicate UTXO when adding to utxo to tx mapping!\n");
      exit(1);
    }
  }
  return 0;
}

int utxo_to_tx_remove(unsigned char *utxo_tx_hash, unsigned int vout) {
  UTXOToTx *entry;

  entry = utxo_to_tx_find_node(utxo_tx_hash, vout);
  if (entry != NULL) {
    HASH_DEL(utxo_to_tx, entry);
    free(entry);
    return 0;
  }

  return 1;
}

int utxo_to_tx_find(unsigned char *dest, unsigned char *utxo_tx_hash, unsigned int vout) {
  UTXOToTx *found_entry;

  found_entry = utxo_to_tx_find_node(utxo_tx_hash, vout);
  if (found_entry != NULL) {
    memcpy(dest, found_entry->tx_hash, TX_HASH_LEN);
    return 0;
  }

  return 1;
}

UTXOToTx *utxo_to_tx_find_node(unsigned char *utxo_tx_hash, unsigned int vout) {
  UTXOPoolKey key;

  memset(&key, 0, sizeof(UTXOPoolKey));
  memcpy(key.tx_hash, utxo_tx_hash, TX_HASH_LEN);
  key.vout = vout;

  return utxo_to_tx_find_node_key(&key);
}

UTXOToTx *utxo_to_tx_find_node_key(UTXOPoolKey *key) {
  UTXOToTx *found_entry;
  HASH_FIND(hh, utxo_to_tx, key, sizeof(UTXOPoolKey), found_entry);
  return found_entry;
}
