#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "base_block.h"
#include "ser_block.h"
#include "crypto.h"

void hash_blockheader(unsigned char *dest, BlockHeader *header) {
  unsigned char *header_buf;
  ssize_t header_size;
  header_buf = ser_blockheader_alloc(&header_size, header);
  hash_sha256(dest, header_buf, header_size);
  free(header_buf);
}

void free_block(Block *block) {
  if (block == NULL)
    return;

  if (block->txs != NULL) {
    for (unsigned int i = 0; i < block->num_txs; i++)
      if (block->txs[i] != NULL)
        free_tx(block->txs[i]);
    free(block->txs);
  }

  free(block);
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
  printf("%s\n%i Transaction%s\n", prefix, block->num_txs, (block->num_txs > 1 ? "s" : ""));
  printf(LINE_BREAK);
  for(unsigned int i=0; i<block->num_txs; i++){
    pretty_print_tx(block->txs[i], prefix);
    printf(LINE_BREAK);
  }
}
