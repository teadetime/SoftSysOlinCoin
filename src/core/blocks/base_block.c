#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "base_block.h"

unsigned char *ser_blockheader(unsigned char *dest, BlockHeader *block_header){
  memcpy(dest, &(block_header->timestamp), sizeof(block_header->timestamp));

  unsigned char *all_tx = dest + sizeof(block_header->timestamp);
  memcpy(all_tx, &(block_header->all_tx), sizeof(block_header->all_tx));

  unsigned char *prev_header_hash = all_tx + sizeof(block_header->all_tx);
  memcpy(prev_header_hash, &(block_header->prev_header_hash), sizeof(block_header->prev_header_hash));

  unsigned char *nonce = prev_header_hash + sizeof(block_header->prev_header_hash);
  memcpy(nonce, &(block_header->nonce), sizeof(block_header->nonce));

  unsigned char* terminate = nonce+sizeof(block_header->nonce);
  return terminate;
}

unsigned char *ser_blockheader_alloc(BlockHeader *block_header){
  unsigned char *data = malloc(sizeof(BlockHeader));
  ser_blockheader(data, block_header);
  return data;
}

void hash_blockheader(unsigned char *dest, BlockHeader *header) {
  unsigned char *header_buf;
  header_buf = ser_blockheader_alloc(header);
  hash_sha256(dest, header_buf, sizeof(BlockHeader));
  free(header_buf);
}

int size_block(Block *block){
  int size = (sizeof(block->num_txs) + sizeof(block->header));
  for(unsigned int i = 0; i < block->num_txs; i++){
      size += size_tx(block->txs[i]);
  }
  return size;
}

unsigned char *ser_block(unsigned char *dest, Block *block){
  memcpy(dest, &(block->num_txs), sizeof(block->num_txs));

  unsigned char *block_header = dest + sizeof(block->num_txs);
  unsigned char *txs = ser_blockheader(block_header, &(block->header));

  for(unsigned int i = 0; i < block->num_txs; i++){
      txs = ser_tx(txs, block->txs[i]);
  }
  unsigned char *end = txs;
  return end;
}

unsigned char *ser_block_alloc(Block *block){
  unsigned char *data = malloc(size_block(block));
  ser_block(data, block);
  return data;
}

void print_block_header(BlockHeader *header, char *prefix){
  char *sub_prefix = malloc(strlen(prefix)+strlen(PRINT_TAB)+1);
  strcpy(sub_prefix, prefix);
  strcat(sub_prefix, PRINT_TAB);
  printf("%sBlockHeader\n", prefix);
  dump_buf(sub_prefix ,"prev_header_hash:", header->prev_header_hash, BLOCK_HASH_LEN);
  printf("%stimestamp: %li\n", sub_prefix, header->timestamp);
  printf("%snonce: %li\n", sub_prefix, header->nonce);
  dump_buf( sub_prefix, "all_tx_hash:", header->all_tx, TX_HASH_LEN);
  free(sub_prefix);
}

void pretty_print_block_header(BlockHeader *header, char *prefix) {
  dump_buf(prefix, "Prev Block: ", header->prev_header_hash, BLOCK_HASH_LEN);
  printf("%sTimestamp: %lu\n", prefix, header->timestamp);
  printf("%sNonce: %lu\n", prefix, header->nonce);
}

void print_block(Block *block, char *prefix){
  char *sub_prefix = malloc(strlen(prefix)+strlen(PRINT_TAB)+1);
  strcpy(sub_prefix, prefix);
  strcat(sub_prefix, PRINT_TAB);
  printf("%sBlock\n", prefix);
  print_block_header(&(block->header), sub_prefix);

  printf("%sNum Txs: %i\n", sub_prefix, block->num_txs);
  printf("%sTransactions\n", sub_prefix);
  for(unsigned int i=0; i<block->num_txs; i++){
    print_tx(block->txs[i], sub_prefix);
  }
  free(sub_prefix);
}

void pretty_print_block(Block *block, char *prefix){
  pretty_print_block_header(&block->header, prefix);
  printf(LINE_BREAK);
  printf("%s%i Transaction%s\n", prefix, block->num_txs, (block->num_txs > 1 ? "s" : ""));
  printf(SOFT_LINE_BREAK);
  for(unsigned int i=0; i<block->num_txs; i++){
    pretty_print_tx(block->txs[i], prefix);
    printf(SOFT_LINE_BREAK);
  }
  printf(LINE_BREAK);
}
