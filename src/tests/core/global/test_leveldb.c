#include "leveldb/c.h"
#include <stdio.h>
#include "utxo_pool.h"
#include "init_db.h"
#include "crypto.h"
#include "ser_tx.h"
#include "blockchain.h"
#include "ser_block.h"

Transaction *_make_tx() {
  Transaction *tx;
  Input *in;
  Output *out;
  mbedtls_ecdsa_context *key_pair;

  tx = malloc(sizeof(Transaction));
  in = malloc(sizeof(Input));
  out = malloc(sizeof(Output));

  memset(in, 0, sizeof(Input));
  key_pair = gen_keys();
  in->pub_key = &(key_pair->private_Q);

  memset(out, 0, sizeof(Output));
  out[0].amt = 99;
  tx->num_inputs = 1;
  tx->inputs = in;
  tx->num_outputs = 1;
  tx->outputs = out;

  return tx;
}
Block *_make_block() {
  Block *block;
  Transaction *tx;

  block = malloc(sizeof(Block));
  tx = malloc(sizeof(Transaction));

  tx->num_inputs = 0;
  tx->inputs = NULL;
  tx->num_outputs = 0;
  tx->outputs = NULL;

  block->num_txs = 1;
  block->txs = &tx;

  block->header.timestamp = 1;
  memset(block->header.all_tx, 1, ALL_TX_HASH_LEN);
  memset(block->header.prev_header_hash, 1, BLOCK_HASH_LEN);
  block->header.nonce = 1;

  return block;
}

int main() {
  create_proj_folders();
  blockchain_init_leveldb(PROD_DB_LOC);
  // destroy_db(&blockchain_db, blockchain_path);
  // blockchain_init_leveldb(PROD_DB_LOC);
  // unsigned int num_blocks;
  // // blockchain_count(&num_blocks);
  // int test = read_chain();
  // unsigned char *hash = read_top_hash();
  //printf("Chain height: %i, Numblocks: %i\n", test, num_blocks);
  //dump_buf("", "KEY: ", hash, BLOCK_HASH_LEN);

}
