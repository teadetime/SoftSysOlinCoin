#include "mempool.h"

void mempool_init() {
  mempool = NULL;
}

Transaction *mempool_add(Transaction *tx) {
  MemPool *new_entry, *found_entry;

  hash_tx(tx, new_entry->id);
  new_entry->tx = tx;

  found_entry = mempool_find_node(new_entry->id);
  if (found_entry == NULL) {
    HASH_ADD(hh, mempool, id, TX_HASH_LEN, new_entry);
    return tx;
  }
  free(new_entry);
  return NULL;
}

Transaction *mempool_remove(Transaction *tx) {
  MemPool *entry;
  char *tx_buf;
  int tx_buf_size;
  unsigned char buf[TX_HASH_LEN];

  hash_tx(tx, buf);
  entry = mempool_find_node(buf);

  if (entry != NULL) {
    HASH_DEL(mempool, entry);
    free(entry);
    return tx;
  }
  return NULL;
}

Transaction *mempool_find(Transaction *tx) {
  MemPool *found_entry;
  unsigned char buf[TX_HASH_LEN];

  hash_tx(tx, buf);
  found_entry = mempool_find_node(buf);
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
