#include <stdlib.h>
#include <string.h>
#include "base_block.h"
#include "ser_block.h"
#include "ser_tx.h"

#define RETURN_SER(data, ret, ptr) \
  { \
    if (ptr != NULL) \
      *ptr = ret; \
    if (ret == -1) { \
      free(data); \
      return NULL; \
    } \
    return data; \
  }

size_t size_ser_blockheader() {
  return sizeof(((BlockHeader*)0)->timestamp) +
    sizeof(((BlockHeader*)0)->all_tx) +
    sizeof(((BlockHeader*)0)->prev_header_hash) +
    sizeof(((BlockHeader*)0)->nonce);
}

ssize_t ser_blockheader(unsigned char *dest, BlockHeader *blockheader) {
  memcpy(dest, &(blockheader->timestamp), sizeof(blockheader->timestamp));

  unsigned char *all_tx = dest + sizeof(blockheader->timestamp);
  memcpy(all_tx, &(blockheader->all_tx), sizeof(blockheader->all_tx));

  unsigned char *prev_header_hash = all_tx + sizeof(blockheader->all_tx);
  memcpy(prev_header_hash, &(blockheader->prev_header_hash),
      sizeof(blockheader->prev_header_hash));

  unsigned char *nonce = prev_header_hash + sizeof(blockheader->prev_header_hash);
  memcpy(nonce, &(blockheader->nonce), sizeof(blockheader->nonce));

  unsigned char* end = nonce + sizeof(blockheader->nonce);
  return end - dest;
}

unsigned char *ser_blockheader_alloc(ssize_t *written, BlockHeader *blockheader) {
  unsigned char *data;
  ssize_t ret;
  data = malloc(size_ser_blockheader());
  ret = ser_blockheader(data, blockheader);
  RETURN_SER(data, ret, written)
}

ssize_t deser_blockheader(BlockHeader *dest, unsigned char *src) {
  memcpy(&dest->timestamp, src, sizeof(((BlockHeader*)0)->timestamp));
  unsigned char *incoming_all_tx = src + sizeof(((BlockHeader*)0)->timestamp);

  memcpy(dest->all_tx, incoming_all_tx, sizeof(((BlockHeader*)0)->all_tx));
  unsigned char *incoming_prev_header_hash = incoming_all_tx +
    sizeof(((BlockHeader*)0)->all_tx);

  memcpy(dest->prev_header_hash, incoming_prev_header_hash,
      sizeof(((BlockHeader*)0)->prev_header_hash));
  unsigned char *incoming_nonce = incoming_prev_header_hash +
    sizeof(((BlockHeader*)0)->prev_header_hash);

  memcpy(&dest->nonce, incoming_nonce, sizeof(((BlockHeader*)0)->nonce));
  unsigned char *end = incoming_nonce + sizeof(((BlockHeader*)0)->nonce);

  return end - src;
}

BlockHeader *deser_blockheader_alloc(ssize_t *read, unsigned char *src) {
  BlockHeader *header;
  ssize_t ret;
  header = malloc(sizeof(BlockHeader));
  ret = deser_blockheader(header, src);
  RETURN_SER(header, ret, read)
}

size_t size_ser_block(Block *block){
  size_t size = sizeof(block->num_txs) + size_ser_blockheader();
  for(unsigned int i = 0; i < block->num_txs; i++)
      size += size_ser_tx(block->txs[i]);
  return size;
}

ssize_t ser_block(unsigned char *dest, Block *block){
  memcpy(dest, &(block->num_txs), sizeof(block->num_txs));

  unsigned char *block_header = dest + sizeof(block->num_txs);
  unsigned char *txs = block_header + ser_blockheader(block_header, &(block->header));

  for(unsigned int i = 0; i < block->num_txs; i++)
      txs += ser_tx(txs, block->txs[i]);
  unsigned char *end = txs;

  return end - dest;
}

unsigned char *ser_block_alloc(ssize_t *written, Block *block){
  unsigned char *data;
  ssize_t ret;
  data = malloc(size_ser_block(block));
  ret = ser_block(data, block);
  RETURN_SER(data, ret, written)
}

ssize_t deser_block(Block *dest, unsigned char *src){
  memcpy(&dest->num_txs, src, sizeof(((Block*)0)->num_txs));
  unsigned char *incoming_header = src + sizeof(((Block*)0)->num_txs);

  unsigned char *incoming_txs = incoming_header +
    deser_blockheader(&dest->header, incoming_header);

  dest->txs = malloc(dest->num_txs * sizeof(Transaction*));
  for(int tx = 0; tx < dest->num_txs; tx++){
    dest->txs[tx] = malloc(sizeof(Transaction));
    incoming_txs += deser_tx(dest->txs[tx], incoming_txs);
  }

  return incoming_txs - src;
}

Block *deser_block_alloc(ssize_t *read, unsigned char *src) {
  Block *block;
  ssize_t ret;
  block = malloc(sizeof(Block));
  ret = deser_block(block, src);
  RETURN_SER(block, ret, read)
}
