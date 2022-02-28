#include "blockchain.h"

void blockchain_init() {
  Block* genesis_block;

  blockchain = NULL;  // Must always start at NULL

  genesis_block = malloc(sizeof(Block));

  // Initialize with a totally empty block
  genesis_block->num_txs = 0;
  genesis_block->txs = NULL;

  genesis_block->header.timestamp = 0;
  genesis_block->header.nonce = 0;
  memset(&(genesis_block->header.all_tx), 0, TX_HASH_LEN);
  memset(&(genesis_block->header.prev_header_hash), 0, BLOCK_HASH_LEN);

  blockchain_add(genesis_block);
}

Block *blockchain_add(Block *block) {
  BlockChain *new_entry, *found_entry;

  new_entry = malloc(sizeof(BlockChain));
  hash_blockheader(new_entry->id, &(block->header));
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