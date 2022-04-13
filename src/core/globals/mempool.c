#include "mempool.h"
#include "crypto.h"
#include "utxo_to_tx.h"

void mempool_init() {
  mempool = NULL;
  utxo_to_tx_init();
}

Transaction *mempool_add(Transaction *tx) {
  MemPool *new_entry, *found_entry;

  new_entry = malloc(sizeof(MemPool));
  hash_tx(new_entry->id, tx);
  new_entry->tx = tx;

  found_entry = mempool_find_node(new_entry->id);
  if (found_entry == NULL) {
    HASH_ADD(hh, mempool, id, TX_HASH_LEN, new_entry);
    utxo_to_tx_add_tx(tx);
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

void print_mempool(MemPool *mempool, char *prefix){
  dump_buf(prefix, "hashmap_id: ", mempool->id, TX_HASH_LEN);
  print_tx(mempool->tx, prefix);
}

void print_mempool_hashmap(char *prefix){
  char *sub_prefix = malloc(strlen(prefix)+strlen(PRINT_TAB)+1);
  strcpy(sub_prefix, prefix);
  strcat(sub_prefix, PRINT_TAB);

  MemPool *s;
  printf("%sMempool Hashmap items(%i):\n", prefix, HASH_COUNT(mempool));
  for (s = mempool; s != NULL; s = s->hh.next) {
    print_mempool(s, sub_prefix);
  }
  free(sub_prefix);
}
