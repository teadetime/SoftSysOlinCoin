#include "blockchain.h"

void blockchain_init() {
  blockchain = NULL;
}

Block *blockchain_add(Block *block) {
  BlockChain *new_entry, *found_entry;

  new_entry = malloc(sizeof(BlockChain));
  hash_blockheader(&(block->header), new_entry->id);
  new_entry->block = block;

  found_entry = blockchain_find_node(new_entry->id);
  if (found_entry == NULL) {
    HASH_ADD(hh, blockchain, id, TX_HASH_LEN, new_entry);
    return block;
  }
  free(new_entry);
  return NULL;
}

Block *blockchain_remove(unsigned char *header_hash) {
  BlockChain *entry;
  Block *block;

  entry = blockchain_find_node(header_hash);
  if (entry != NULL) {
    block = entry->block;
    HASH_DEL(blockchain, entry);
    free(entry);
    return block;
  }

  return NULL;
}

Block *blockchain_find(unsigned char *header_hash) {
  BlockChain *found_entry;

  found_entry = blockchain_find_node(header_hash);
  if (found_entry != NULL) {
    return found_entry->block;
  }

  return NULL;
}

BlockChain *blockchain_find_node(unsigned char *header_hash) {
  BlockChain *found_entry;
  HASH_FIND(hh, blockchain, header_hash, BLOCK_HASH_LEN, found_entry);
  return found_entry;
}
