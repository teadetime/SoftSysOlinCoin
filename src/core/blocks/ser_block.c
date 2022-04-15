#include <stdlib.h>
#include <string.h>
#include "base_block.h"
#include "ser_block.h"
#include "ser_tx.h"

size_t size_ser_blockheader() {
  return sizeof(((BlockHeader*)0)->timestamp) +
    sizeof(((BlockHeader*)0)->all_tx) +
    sizeof(((BlockHeader*)0)->prev_header_hash) +
    sizeof(((BlockHeader*)0)->nonce);
}

size_t ser_blockheader(unsigned char *dest, BlockHeader *block_header) {
  memcpy(dest, &(block_header->timestamp), sizeof(block_header->timestamp));

  unsigned char *all_tx = dest + sizeof(block_header->timestamp);
  memcpy(all_tx, &(block_header->all_tx), sizeof(block_header->all_tx));

  unsigned char *prev_header_hash = all_tx + sizeof(block_header->all_tx);
  memcpy(prev_header_hash, &(block_header->prev_header_hash),
      sizeof(block_header->prev_header_hash));

  unsigned char *nonce = prev_header_hash + sizeof(block_header->prev_header_hash);
  memcpy(nonce, &(block_header->nonce), sizeof(block_header->nonce));

  unsigned char* end = nonce + sizeof(block_header->nonce);
  return end - dest;
}

unsigned char *ser_blockheader_alloc(BlockHeader *block_header) {
  unsigned char *data;
  data = malloc(size_ser_blockheader());
  ser_blockheader(data, block_header);
  return data;
}

size_t deser_blockheader(BlockHeader *dest, unsigned char *src) {
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

BlockHeader *deser_blockheader_alloc(unsigned char *src) {
  BlockHeader *header;
  header = malloc(sizeof(BlockHeader));
  deser_blockheader(header, src);
  return header;
}

size_t size_ser_block(Block *block){
  size_t size = sizeof(block->num_txs) + size_ser_blockheader();
  for(unsigned int i = 0; i < block->num_txs; i++)
      size += size_ser_tx(block->txs[i]);
  return size;
}

size_t ser_block(unsigned char *dest, Block *block){
  memcpy(dest, &(block->num_txs), sizeof(block->num_txs));

  unsigned char *block_header = dest + sizeof(block->num_txs);
  unsigned char *txs = block_header + ser_blockheader(block_header, &(block->header));

  for(unsigned int i = 0; i < block->num_txs; i++)
      txs += ser_tx(txs, block->txs[i]);
  unsigned char *end = txs;

  return end - dest;
}

unsigned char *ser_block_alloc(Block *block){
  unsigned char *data;
  data = malloc(size_ser_block(block));
  ser_block(data, block);
  return data;
}

size_t deser_block(Block *dest, unsigned char *src){
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

Block *deser_block_alloc(unsigned char *src) {
  Block *block;
  block = malloc(sizeof(Block));
  deser_block(block, src);
  return block;
}
