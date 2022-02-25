#pragma once

#include "constants.h"
#include "uthash.h"
#include "base_block.h"

typedef struct BlockChain {
  unsigned char id[BLOCK_HASH_LEN];
  Block block;
} BlockChain;

BlockChain *blockchain;
