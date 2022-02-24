#include "mempool.h"

void mempool_init() {
  mempool = NULL;
}

Transaction* mempool_add(Transaction *tx) {
  MemPool *new_entry, *found_entry;

  new_entry = malloc(sizeof(MemPool));
  hash_tx(tx, new_entry->id);
  new_entry->tx = tx;

  found_entry = mempool_find(new_entry->id);
  if (found_entry == NULL) {
    HASH_ADD_STR(mempool, id, new_entry);
    return tx;
  } else {
    // In practice this should never happen
    free(new_entry);
    return NULL;
  }
}

Transaction* mempool_remove(Transaction *tx) {
  MemPool *entry;
  char buf[TX_HASH_LEN];

  hash_tx(tx, buf);
  entry = mempool_find(buf);

  if (entry != NULL) {
    HASH_DEL(mempool, entry);
    return tx;
  }
  return NULL;
}

MemPool* mempool_find(char *tx_hash) {
  MemPool *found_entry;
  HASH_FIND_STR(mempool, tx_hash, found_entry);
  return found_entry;
}
