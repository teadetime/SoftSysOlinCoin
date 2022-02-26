#pragma once

#include "constants.h"
#include "uthash.h"
#include "base_block.h"

typedef struct BlockChain {
  unsigned char id[BLOCK_HASH_LEN];
  Block *block;
  UT_hash_handle hh;
} BlockChain;

BlockChain *blockchain;

void blockchain_init();
Block *blockchain_add(Block *block);
Block *blockchain_remove(unsigned char *header_hash);
Block *blockchain_find(unsigned char *header_hash);
BlockChain *blockchain_find_node(unsigned char *header_hash);
