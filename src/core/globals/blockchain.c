#include <stdio.h>
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
  memset(genesis_block->header.all_tx, 0, TX_HASH_LEN);
  memset(genesis_block->header.prev_header_hash, 0, BLOCK_HASH_LEN);

  blockchain_add(genesis_block);
}

Block *blockchain_add(Block *block) {
  BlockChain *new_entry, *found_entry;

  new_entry = malloc(sizeof(BlockChain));
  hash_blockheader(new_entry->id, &(block->header));
  new_entry->block = block;

  found_entry = blockchain_find_node(new_entry->id);
  if (found_entry == NULL) {
    HASH_ADD(hh, blockchain, id, BLOCK_HASH_LEN, new_entry);
    chain_height += 1;
    hash_blockheader(top_block_header_hash, &(block->header));
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

void print_blockchain(BlockChain *blockchain_node, char *prefix){
  dump_buf(prefix, "hashmap_id: ", blockchain_node->id, BLOCK_HASH_LEN);
  print_block(blockchain_node->block, prefix);
}


void print_blockchain_hashmap(char *prefix){
  char *sub_prefix = malloc(strlen(prefix)+strlen(PRINT_TAB)+1);
  strcpy(sub_prefix, prefix);
  strcat(sub_prefix, PRINT_TAB);

  BlockChain *s;
  printf("%sBlockchain Hashmap items(%i):\n", prefix, HASH_COUNT(blockchain));
  for (s = blockchain; s != NULL; s = s->hh.next) {
    print_blockchain(s, sub_prefix);
  }
  free(sub_prefix);
}