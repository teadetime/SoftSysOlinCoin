#include "fixtures_block.h"
#include "fixtures_tx.h"
#include "crypto.h"
#include "base_block.h"

#include <string.h>

#define NUM_TXS 4

static void set_header(BlockHeader *header) {
  header->timestamp = 100;
  header->nonce = 123;
  memset(header->prev_header_hash, 0xFA, BLOCK_HASH_LEN);
  memset(header->all_tx, 0xAF, ALL_TX_HASH_LEN);
}

int fixture_setup_unlinked_header(void **state){
  BlockHeader *header;

  header = malloc(sizeof(BlockHeader));
  set_header(header);

  *state = header;

  return 0;
}

int fixture_teardown_unlinked_header(void **state) {
  free(*state);
  return 0;
}

int fixture_setup_unlinked_block(void **state) {
  Block *block;
  Transaction *coinbase;
  unsigned char tx_hashes[TX_HASH_LEN * NUM_TXS];

  block = malloc(sizeof(Block));
  set_header(&block->header);
  block->num_txs = NUM_TXS;
  block->txs = malloc(NUM_TXS * sizeof(Transaction*));
  for (unsigned int i = 0; i < NUM_TXS; i++)
    fixture_setup_unlinked_tx((void**)block->txs + i);

  // Make first tx a coinbase by removing inputs
  coinbase = block->txs[0];
  for (unsigned int i = 0; i < coinbase->num_inputs; i++)
    mbedtls_ecp_point_free(coinbase->inputs[i].pub_key);
  free(coinbase->inputs);
  coinbase->inputs = NULL;
  coinbase->num_inputs = 0;

  // Build all_tx_hash
  for (unsigned int i = 0; i < NUM_TXS; i++)
    hash_tx(tx_hashes + i * TX_HASH_LEN, block->txs[i]);
  hash_sha256(block->header.all_tx, tx_hashes, TX_HASH_LEN * NUM_TXS);

  *state = block;

  return 0;
}

int fixture_teardown_unlinked_block(void **state) {
  Block *block;

  block = *state;
  for (unsigned int i = 0; i < block->num_txs; i++)
    free_tx(block->txs[i]);
  free(block->txs);
  free(block);

  return 0;
}
