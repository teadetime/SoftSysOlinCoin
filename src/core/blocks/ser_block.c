#include <stdlib.h>
#include <string.h>
#include "base_block.h"
#include "ser_block.h"
#include "ser_tx.h"

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

int deser_blockheader(BlockHeader *dest_header, unsigned char *blockheader_data){
  memcpy(&dest_header->timestamp, blockheader_data, sizeof(((BlockHeader*)0)->timestamp));
  unsigned char *incoming_all_tx = blockheader_data + sizeof(((BlockHeader*)0)->timestamp);

  memcpy(dest_header->all_tx, incoming_all_tx, sizeof(((BlockHeader*)0)->all_tx));
  unsigned char *incoming_prev_header_hash = incoming_all_tx + sizeof(((BlockHeader*)0)->all_tx);
  
  memcpy(dest_header->prev_header_hash, incoming_prev_header_hash, sizeof(((BlockHeader*)0)->prev_header_hash));
  unsigned char *incoming_nonce = incoming_prev_header_hash + sizeof(((BlockHeader*)0)->prev_header_hash);

  memcpy(&dest_header->nonce, incoming_nonce, sizeof(((BlockHeader*)0)->nonce));
  unsigned char *end = incoming_nonce + sizeof(((BlockHeader*)0)->nonce);
  
  return 1; // Seems like serialization should get an input size and it should check to make sure it is what is expected
}

int deser_block(Block *dest_block, unsigned char *block_data){
  // How can we alc the size that this should be?
  memcpy(&dest_block->num_txs, block_data, sizeof(((Block*)0)->num_txs));
  unsigned char *incoming_header = block_data + sizeof(((Block*)0)->num_txs);

  deser_blockheader(&dest_block->header, incoming_header);
  size_t header_sz = sizeof((BlockHeader*)0)->nonce + sizeof((BlockHeader*)0)->all_tx
   + sizeof((BlockHeader*)0)->prev_header_hash + sizeof((BlockHeader*)0)->timestamp;
  unsigned char *incoming_txs = incoming_header + header_sz;

  dest_block->txs = malloc(dest_block->num_txs * sizeof(Transaction*));
  for(int tx = 0; tx < dest_block->num_txs; tx++){
    // This should be refactored to return the next address in memory since these are variable sized to avoid calling size tx twice
    dest_block->txs[tx] = deser_tx(incoming_txs);
    incoming_txs += size_tx(dest_block->txs[tx]);
  }
  return 1;
}
