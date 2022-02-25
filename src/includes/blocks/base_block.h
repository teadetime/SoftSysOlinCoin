#ifndef BASE_BLOCK_H
#define BASE_BLOCK_H

#include "constants.h"
#include "base_tx.h"

typedef struct BlockHeader{
    unsigned long timestamp;
    unsigned char all_tx[TX_HASH_LEN];
    unsigned char prev_header_hash[BLOCK_HASH_LEN];
    int nonce;
} BlockHeader;

typedef struct Block{
    unsigned int num_txs;
    BlockHeader header;
    Transaction *txs;
} Block;

#endif

char *ser_blockheader(char *dest, BlockHeader *block_header);
char *ser_blockheader_alloc(BlockHeader *block_header);

int size_block(Block *Block);
char *ser_block(char *dest, Block *block);
char *ser_block_alloc(Block *block);