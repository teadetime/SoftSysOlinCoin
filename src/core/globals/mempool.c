#include "mempool.h"

void mempool_init() {
  mempool = NULL;
}

Transaction *mempool_add(Transaction *tx) {
  MemPool *new_entry, *found_entry;

  new_entry = malloc(sizeof(MemPool));
  hash_tx(new_entry->id, tx);
  new_entry->tx = tx;

  found_entry = mempool_find_node(new_entry->id);
  if (found_entry == NULL) {
    HASH_ADD(hh, mempool, id, TX_HASH_LEN, new_entry);
    return tx;
  }
  free(new_entry);
  return NULL;
}

Transaction *mempool_remove(unsigned char *tx_hash) {
  MemPool *entry;
  Transaction *tx;

  entry = mempool_find_node(tx_hash);
  if (entry != NULL) {
    tx = entry->tx;
    HASH_DEL(mempool, entry);
    free(entry);
    return tx;
  }

  return NULL;
}

Transaction *mempool_find(unsigned char *tx_hash) {
  MemPool *found_entry;

  found_entry = mempool_find_node(tx_hash);
  if (found_entry != NULL) {
    return found_entry->tx;
  }

  return NULL;
}

MemPool *mempool_find_node(unsigned char *tx_hash) {
  MemPool *found_entry;
  HASH_FIND(hh, mempool, tx_hash, TX_HASH_LEN, found_entry);
  return found_entry;
}
