#include <stdlib.h>
#include "minunit.h"
#include "handle_block.h"
#include "create_block.h"
#include "mempool.h"
#include "utxo_pool.h"
#include "blockchain.h"
#include "wallet_pool.h"
#include "utxo_to_tx.h"
#include "crypto.h"
#include "init_db.h"

int tests_run = 0;

mbedtls_ecdsa_context *last_key_pair;
Transaction *_make_tx() {
  Transaction *tx;
  Input *in;
  Output *out;
  mbedtls_ecdsa_context *key_pair;
  mbedtls_ecdsa_context *output_key_pair;
  tx = malloc(sizeof(Transaction));
  in = malloc(sizeof(Input));
  out = malloc(sizeof(Output));

  memset(in, 0, sizeof(Input));
  key_pair = gen_keys();
  in->pub_key = &(key_pair->private_Q);
  in->sig_len = 0;

  memset(out, 0, sizeof(Output));

  output_key_pair = gen_keys();
  hash_pub_key(out->public_key_hash, output_key_pair);
  last_key_pair = output_key_pair;
  tx->num_inputs = 1;
  tx->inputs = in;
  tx->num_outputs = 1;
  tx->outputs = out;

  return tx;
}

void _fill_mempool(){
  Transaction *input_tx, *tx1;
  input_tx = _make_tx();
  input_tx->outputs[0].amt = 100;
  utxo_pool_init_leveldb();
  utxo_pool_add_leveldb(input_tx, 0);

  mbedtls_ecdsa_context *input_tx_context = malloc(sizeof(mbedtls_ecdsa_context));
  memcpy(input_tx_context, last_key_pair, sizeof(mbedtls_ecdsa_context));

  tx1 = _make_tx();
  hash_tx(tx1->inputs[0].prev_tx_id, input_tx);
  tx1->inputs[0].prev_utxo_output = 0;
  tx1->inputs[0].pub_key = &input_tx_context->private_Q;
  tx1->outputs[0].amt = 90;

  unsigned char temp_hash[TX_HASH_LEN];
  hash_tx(temp_hash, tx1);
  tx1->inputs[0].sig_len = write_sig(tx1->inputs[0].signature, SIGNATURE_LEN, temp_hash, TX_HASH_LEN, input_tx_context);
  mempool_init();
  mempool_add(tx1);
}

static char  *test_update_local_blockchain() {
  blockchain_init_leveldb();
  wallet_init_leveldb();
  _fill_mempool();
  Block *good_block = mine_block();
  Block *found_block = NULL;
  unsigned int prev_height = chain_height;
  update_local_blockchain(good_block);
  unsigned char block_hash[BLOCK_HASH_LEN];
  hash_blockheader(block_hash, &(good_block->header));
  int ret_find = blockchain_find_leveldb(&found_block, block_hash);
  mu_assert(
    "Top Block Hash not set",
    memcmp(top_block_header_hash, block_hash, BLOCK_HASH_LEN) == 0
  );
  mu_assert(
    "Chain height did not increase by 1",
    chain_height == prev_height+1
  );
  mu_assert(
    "Unable to locate block in blockchain",
    ret_find == 0
  );
  mu_assert(
    "Find did not return correct block",
    memcmp(&found_block->header, &good_block->header, sizeof(BlockHeader)) == 0
  );
  destroy_db(&utxo_pool_db, utxo_pool_path);
  destroy_db(&blockchain_db, blockchain_path);
  destroy_wallet();
  return NULL;
}

static char  *test_update_utxo_pool() {
  blockchain_init_leveldb();
  wallet_init_leveldb();
  _fill_mempool();
  Block *good_block = mine_block();
  unsigned int prev_utxo_size = 99;
  utxo_pool_count(&prev_utxo_size);
  unsigned int prev_wallet_size = 99;
  wallet_pool_count(&prev_wallet_size);
  update_UTXO_pool_and_wallet_pool(good_block);

  unsigned int new_utxo_size = 99;
  utxo_pool_count(&new_utxo_size);
  unsigned int new_wallet_size = 99;
  wallet_pool_count(&new_wallet_size);
  mu_assert(
    "UTXO pool didn't change by expected amount",
    prev_utxo_size+1 == new_utxo_size
  );
  mu_assert(
    "Wallet pool didn't change by expected amount",
    prev_wallet_size+1 == new_wallet_size
  );
  destroy_db(&utxo_pool_db, utxo_pool_path);
  destroy_db(&blockchain_db, blockchain_path);
  destroy_wallet();
  return NULL;
}

static char  *test_update_mempool() {
  blockchain_init_leveldb();
  _fill_mempool();
  wallet_init_leveldb();
  Block *good_block = mine_block();

  unsigned int prev_mempool_size = HASH_COUNT(mempool);
  unsigned int prev_mapping_size = HASH_COUNT(utxo_to_tx);
  update_mempool(good_block);
  mu_assert(
    "Mempool didn't change by expected amount",
    prev_mempool_size-1 == HASH_COUNT(mempool)
  );
  mu_assert(
    "UTXO to tx mapping didn't change by expected amount",
    prev_mapping_size-1 == HASH_COUNT(utxo_to_tx)
  );
  destroy_db(&utxo_pool_db, utxo_pool_path);
  destroy_db(&blockchain_db, blockchain_path);
  destroy_wallet();
  return NULL;
}

static char  *test_accept_block() {
  blockchain_init_leveldb();
  wallet_init_leveldb();
  _fill_mempool();
  unsigned int prev_height = chain_height;
  Block *good_block = mine_block();
  Block *found_block = NULL;
  unsigned int prev_utxo_size;
  utxo_pool_count(&prev_utxo_size);
  unsigned int prev_wallet_size;
  wallet_pool_count(&prev_wallet_size);
  unsigned int prev_mempool_size = HASH_COUNT(mempool);
  unsigned int prev_mapping_size = HASH_COUNT(utxo_to_tx);
  unsigned char block_hash[BLOCK_HASH_LEN];
  hash_blockheader(block_hash, &(good_block->header));
  accept_block(good_block);

  unsigned int new_utxo_size = 99;
  utxo_pool_count(&new_utxo_size);
  unsigned int new_wallet_size = 99;
  wallet_pool_count(&new_wallet_size);
  int ret_find = blockchain_find_leveldb(&found_block, block_hash);
  mu_assert(
    "Accept: Top Block Hash not set",
    memcmp(top_block_header_hash, block_hash, BLOCK_HASH_LEN) == 0
  );
  mu_assert(
    "Accepts: Chain height did not increase by 1",
    chain_height == prev_height+1
  );
  mu_assert(
    "Accept: Unable to locate block in blockchain",
    ret_find == 0
  );
  mu_assert(
    "Find did not return correct block",
    memcmp(&found_block->header, &good_block->header, sizeof(BlockHeader)) == 0
  );
  mu_assert(
    "Accept: UTXO pool didn't change by expected amount",
    prev_utxo_size+1 == new_utxo_size
  );
  mu_assert(
    "Accept: Wallet pool didn't change by expected amount",
    prev_wallet_size+1 == new_wallet_size
  );
  mu_assert(
    "Accept: Mempool didn't change by expected amount",
    prev_mempool_size-1 == HASH_COUNT(mempool)
  );
  mu_assert(
    "Accept: UTXO to tx mapping didn't change by expected amount",
    prev_mapping_size-1 == HASH_COUNT(utxo_to_tx)
  );
  destroy_db(&utxo_pool_db, utxo_pool_path);
  destroy_db(&blockchain_db, blockchain_path);
  destroy_wallet();
  return NULL;
}


static char  *test_handle_block() {
  blockchain_init_leveldb();
  wallet_init_leveldb();
  _fill_mempool();
  unsigned int prev_height = chain_height;
  Block *good_block = mine_block();
  Block *found_block = NULL;
  unsigned int prev_utxo_size;
  utxo_pool_count(&prev_utxo_size);
  unsigned int prev_wallet_size;
  wallet_pool_count(&prev_wallet_size);
  unsigned int prev_mempool_size = HASH_COUNT(mempool);
  unsigned int prev_mapping_size = HASH_COUNT(utxo_to_tx);
  unsigned char block_hash[BLOCK_HASH_LEN];
  hash_blockheader(block_hash, &(good_block->header));
  handle_new_block(good_block);
  int ret_find = blockchain_find_leveldb(&found_block, block_hash);

  unsigned int new_utxo_size = 99;
  utxo_pool_count(&new_utxo_size); 
  unsigned int new_wallet_size = 99;
  wallet_pool_count(&new_wallet_size);
  // Accept tests because accept only runs if validation pasts
  mu_assert(
    "Handle: Top Block Hash not set",
    memcmp(top_block_header_hash, block_hash, BLOCK_HASH_LEN) == 0
  );
  mu_assert(
    "Handle: Chain height did not increase by 1",
    chain_height == prev_height+1
  );
  mu_assert(
    "Handle: Unable to locate block in blockchain",
    ret_find == 0
  );
  mu_assert(
    "Find did not return correct block",
    memcmp(&found_block->header, &good_block->header, sizeof(BlockHeader)) == 0
  );
  mu_assert(
    "Handle: UTXO pool didn't change by expected amount",
    prev_utxo_size+1 == new_utxo_size
  );
  mu_assert(
    "Handle: Wallet pool didn't change by expected amount",
    prev_wallet_size+1 == new_wallet_size
  );
  mu_assert(
    "Handle: Mempool didn't change by expected amount",
    prev_mempool_size-1 == HASH_COUNT(mempool)
  );
  mu_assert(
    "Handle: UTXO to tx mapping didn't change by expected amount",
    prev_mapping_size-1 == HASH_COUNT(utxo_to_tx)
  );
  destroy_db(&utxo_pool_db, utxo_pool_path);
  destroy_wallet();
  destroy_db(&blockchain_db, blockchain_path);
  return NULL;
}

static char *all_tests() {
  //wallet_init();
  mu_run_test(test_update_local_blockchain);
  mu_run_test(test_update_utxo_pool);
  mu_run_test(test_update_mempool);
  mu_run_test(test_accept_block);
  mu_run_test(test_handle_block);

  return NULL;
}

int main() {
  char *result = all_tests();
  if (result != NULL) {
    printf("%s\n", result);
  } else {
    printf("handle_block.c passing!\n");
  }
  printf("Tests run: %d\n", tests_run);

  return result != 0;
}
