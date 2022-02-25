#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "base_block.h"

char * ser_BlockHeader(BlockHeader *block_header, char* dest){
    memcpy(dest, &(block_header->timestamp), sizeof(block_header->timestamp));

    char * all_tx = dest+sizeof(block_header->timestamp);
    memcpy(all_tx, &(block_header->all_tx), sizeof(block_header->all_tx));

    char * prev_header_hash = all_tx+sizeof(block_header->all_tx);
    memcpy(prev_header_hash, &(block_header->prev_header_hash), sizeof(block_header->prev_header_hash));

    char * nonce = prev_header_hash+sizeof(block_header->prev_header_hash);
    memcpy(prev_header_hash, &(block_header->nonce), sizeof(block_header->nonce));

    char* terminate = nonce+sizeof(block_header->nonce);
    return terminate;
}

char * ser_BlockHeader_alloc(BlockHeader *block_header){
    char * data = malloc(sizeof(BlockHeader));
    memcpy(data, &(block_header->timestamp), sizeof(block_header->timestamp));

    char * all_tx = data+sizeof(block_header->timestamp);
    memcpy(all_tx, &(block_header->all_tx), sizeof(block_header->all_tx));

    char * prev_header_hash = all_tx+sizeof(block_header->all_tx);
    memcpy(prev_header_hash, &(block_header->prev_header_hash), sizeof(block_header->prev_header_hash));

    char * nonce = prev_header_hash+sizeof(block_header->prev_header_hash);
    memcpy(prev_header_hash, &(block_header->nonce), sizeof(block_header->nonce));

    return data;
}

int size_block(Block *block){
    int size = (sizeof(block->num_txs) + sizeof(block->header));
    for(int i=0; i<block->num_txs; i++){
        size += size_tx(&(block->txs[i]));
    }
    return size;
}

char * ser_Block(Block *block){
    char * data = malloc(size_block(block));
    memcpy(data, block->num_txs, sizeof(block->num_txs));

    char * block_header = data+sizeof(block->num_txs);
    char * txs = ser_BlockHeader(&(block->header), block_header);

    for(int i=0; i<block->num_txs;i++){
        char *txs = ser_tx(&(block->txs[i]), txs);
    }
    char * terminate = txs;
  return data;
}