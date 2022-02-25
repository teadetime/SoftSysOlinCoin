#include "utxo_pool.h"

void utxo_pool_init() {
  utxo_pool = NULL;
}

UTXO *utxo_pool_add(Transaction *tx, unsigned int vout) {
  UTXOPool *new_entry, *found_entry;
  UTXO *utxo;

  new_entry = malloc(sizeof(UTXOPool));

  // Build key
  memset(&(new_entry->id), 0, sizeof(UTXOPoolKey));
  hash_tx(tx, new_entry->id.tx_hash);
  new_entry->id.vout = vout;

  // Build new UTXO
  utxo = malloc(sizeof(UTXO));
  utxo->amt = tx->outputs[vout].amt;
  memcpy(utxo->public_key_hash, tx->outputs[vout].public_key_hash, LOCK_SCRIPT_LEN);
  utxo->spent = 0;

  new_entry->utxo = utxo;

  found_entry = utxo_pool_find_node_key(&(new_entry->id));
  if (found_entry == NULL) {
    HASH_ADD(hh, utxo_pool, id, sizeof(UTXOPoolKey), new_entry);
    return utxo;
  } else {
    free(new_entry->utxo);
    free(new_entry);
    return NULL;
  }
}

UTXO *utxo_pool_remove(unsigned char *tx_hash, unsigned int vout) {
  UTXOPool *entry;
  UTXO *utxo;

  entry = utxo_pool_find_node(tx_hash, vout);
  if (entry != NULL) {
    utxo = entry->utxo;
    HASH_DEL(utxo_pool, entry);
    free(entry);
    return utxo;
  }

  return NULL;
}

UTXO *utxo_pool_find(unsigned char *tx_hash, unsigned int vout) {
  UTXOPool *found_entry;

  found_entry = utxo_pool_find_node(tx_hash, vout);
  if (found_entry != NULL) {
    return found_entry->utxo;
  }

  return NULL;
}

UTXOPool *utxo_pool_find_node(unsigned char *tx_hash, unsigned int vout) {
  UTXOPoolKey key;

  memset(&key, 0, sizeof(UTXOPoolKey));
  memcpy(key.tx_hash, tx_hash, TX_HASH_LEN);
  key.vout = vout;

  return utxo_pool_find_node_key(&key);
}

UTXOPool *utxo_pool_find_node_key(UTXOPoolKey *key) {
  UTXOPool *found_entry;
  HASH_FIND(hh, utxo_pool, key, sizeof(UTXOPoolKey), found_entry);
  return found_entry;
}
