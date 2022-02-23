#ifndef BASE_BLOCK_H
#define BASE_BLOCK_H

#include "constants.h"
#include "base_tx.h"

typedef struct BlockHeader{
    unsigned long timestamp;
    char all_tx[TX_HASH_LEN];
    char prev_header_hash[BLOCK_HASH_LEN];
    int nonce;
} BlockHeader;

typedef struct Block{
    int num_txs;
    BlockHeader header;
    Transaction *txs;
} Block;

#endif
