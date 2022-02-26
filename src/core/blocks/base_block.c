#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "base_block.h"

char *ser_blockheader(char *dest, BlockHeader *block_header){
  memcpy(dest, &(block_header->timestamp), sizeof(block_header->timestamp));

  char *all_tx = dest + sizeof(block_header->timestamp);
  memcpy(all_tx, &(block_header->all_tx), sizeof(block_header->all_tx));

  char *prev_header_hash = all_tx + sizeof(block_header->all_tx);
  memcpy(prev_header_hash, &(block_header->prev_header_hash), sizeof(block_header->prev_header_hash));

  char *nonce = prev_header_hash + sizeof(block_header->prev_header_hash);
  memcpy(nonce, &(block_header->nonce), sizeof(block_header->nonce));

  char* terminate = nonce+sizeof(block_header->nonce);
  return terminate;
}

char *ser_blockheader_alloc(BlockHeader *block_header){
  char *data = malloc(sizeof(BlockHeader));
  ser_blockheader(data, block_header);
  return data;
}

void hash_blockheader(BlockHeader *header, unsigned char *buf) {
  char *header_buf;
  header_buf = ser_blockheader_alloc(header);
  hash_sha256(buf, header_buf, sizeof(BlockHeader));
  free(header_buf);
}

int size_block(Block *block){
  int size = (sizeof(block->num_txs) + sizeof(block->header));
  for(int i = 0; i < block->num_txs; i++){
      size += size_tx(&(block->txs[i]));
  }
  return size;
}

char *ser_block(char *dest, Block *block){
  memcpy(dest, block->num_txs, sizeof(block->num_txs));

  char *block_header = dest + sizeof(block->num_txs);
  char *txs = ser_blockheader(&(block->header), block_header);

  for(int i = 0; i < block->num_txs; i++){
      char *txs = ser_tx(&(block->txs[i]), txs);
  }
  char *end = txs;
  return end;
}

char *ser_block_alloc(Block *block){
  char *data = malloc(size_block(block));
  ser_block(data, block);
  return data;
}
