#include <stdio.h>
#include "utxo_pool.h"
#include "double_spend_set.h"

void double_spend_set_init(UTXOPool **set) {
  *set = NULL;
}

int double_spend_add(UTXOPool *set, unsigned char *tx_id, unsigned int vout) {
  UTXOPool *new_entry, *found_entry;
  UTXO *utxo;

  new_entry = malloc(sizeof(UTXOPool));

  // Build key
  memset(&(new_entry->id), 0, sizeof(UTXOPoolKey));
  memcpy(new_entry->id.tx_hash, tx_id, TX_HASH_LEN);
  new_entry->id.vout = vout;

  // Build new UTXO
  // utxo = malloc(sizeof(UTXO));
  // utxo->amt = tx->outputs[vout].amt;
  // memcpy(utxo->public_key_hash, tx->outputs[vout].public_key_hash, PUB_KEY_HASH_LEN);
  // utxo->spent = 0;

  new_entry->utxo = NULL; //utxo;

  found_entry = double_spend_find_node_key(set, &(new_entry->id));
  if (found_entry == NULL) {
    HASH_ADD(hh, set, id, sizeof(UTXOPoolKey), new_entry);
    return 0;
  } else {
    free(new_entry);
    return 1;
  }
}

UTXOPool *double_spend_find_node(UTXOPool *set, unsigned char *tx_hash, unsigned int vout) {
  UTXOPoolKey key;
  memset(&key, 0, sizeof(UTXOPoolKey));
  memcpy(key.tx_hash, tx_hash, TX_HASH_LEN);
  key.vout = vout;

  return double_spend_find_node_key(set, &key);
}

UTXOPool *double_spend_find_node_key(UTXOPool *set, UTXOPoolKey *key) {
  UTXOPool *found_entry;
  HASH_FIND(hh, set, key, sizeof(UTXOPoolKey), found_entry);
  return found_entry;
}

void delete_double_spend_set(UTXOPool *set){
  UTXOPool *current_utxo, *tmp;
  HASH_ITER(hh, set, current_utxo, tmp) {
    HASH_DEL(set, current_utxo);  /* delete; users advances to next */
    free(current_utxo);             /* Free the mallocced data */
  }
}
