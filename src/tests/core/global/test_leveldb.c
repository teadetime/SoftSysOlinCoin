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

  // blockchain_init_leveldb();
  // unsigned int count;
  // blockchain_count(&count);
  // printf("After Blockchain init: %i", count);

  Block* block = _make_block();
  //print_block(block, "");
  ssize_t sz ;//= size_ser_block(block);
  unsigned char* test = ser_block_alloc(&sz, block);
  free(test);
  //blockchain_add_leveldb(block);
  // blockchain_count(&count);

  // printf("After Blockchain init: %i", count);
  //destroy_db(&blockchain_db, blockchain_path);
  // int ret;
  // UTXO *found = NULL;
  // utxo_pool_init_leveldb();
  // // leveldb_t *db1 = NULL;
  // // int success = open_or_create_db(&db1, utxo_pool_path);

  // Transaction *tx;
  // tx = _make_tx();
  // ret = utxo_pool_add_leveldb(tx, 0);


  // printf("Add Return Value: %i\n", ret);
  // unsigned char tx_hash[TX_HASH_LEN];
  // hash_tx(tx_hash, tx);
  // ret = utxo_pool_find_leveldb(&found, tx_hash, 0);

  // printf("Find Return Value: %i\n", ret);
  // if(ret == 0){
  //   print_utxo(found, "");
  //   free(found);
  // }


  // print_utxo_hashmap("");

  // ret = utxo_pool_remove_leveldb(tx_hash, 0);

  // unsigned int entries;
  // int count_ret = utxo_pool_count(&entries);
  // printf("count Return Value: %i, counted: %i\n", count_ret, entries);
  return(0);
}
