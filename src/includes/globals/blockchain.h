#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include "constants.h"
#include "uthash.h"
#include "base_block.h"

typedef struct BlockChain {
    char id[BLOCK_HASH_LEN];
    Block block;
} BlockChain;

BlockChain *blockchain;

#endif
