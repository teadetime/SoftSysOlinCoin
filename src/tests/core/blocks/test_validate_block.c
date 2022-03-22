#include <stdlib.h>
#include "minunit.h"
#include "validate_block.h"
#include "create_block.h"
#include "mempool.h"
#include "utxo_pool.h"
#include "sign_tx.h"
#include "blockchain.h"
#include "wallet_pool.h"

int tests_run = 0;

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
  utxo_pool_init();
  utxo_pool_add(input_tx, 0);

  tx1 = _make_tx();
  hash_tx(tx1->inputs[0].prev_tx_id, input_tx);
  tx1->inputs[0].prev_utxo_output = 0;
  tx1->outputs[0].amt = 90; 
  mempool_init();
  mempool_add(tx1);
}

static char  *test_coinbase_tx() {
  _fill_mempool();
  Block *test_block = create_block_alloc();
  
  mu_assert(
    "Incorrect Coinbase TX",
    validate_coinbase_tx(test_block->txs, test_block->num_txs) == 0
  );
  return NULL;
}

static char  *test_validate_txs() {
  _fill_mempool();
  Block *test_block = create_block_alloc();
  
  mu_assert(
    "Some transactions invalid",
    validate_txs(test_block->txs, test_block->num_txs) == 0
  );
  return NULL;
}

static char  *test_validate_prev_block() {
  _fill_mempool();
  Block *test_block = create_block_alloc();
  mu_assert(
    "Previous block hash doesn't exist or isn't top block",
    validate_prev_block_exists(test_block) == 0
  );
  return NULL;
}

static char  *test_validate_all_tx() {
  _fill_mempool();
  Block *test_block = create_block_alloc();
  mu_assert(
    "Previous block hash doesn't exist or isn't top block",
    validate_all_tx_hash(test_block) == 0
  );
  return NULL;
}

static char  *test_validate_whole_block() {
  _fill_mempool();
  Block *test_block = create_block_alloc();
  mu_assert(
    "Block hash is correct before mining",
    validate_block(test_block) == 1
  );

  Block *good_block = mine_block();
    mu_assert(
    "Block mined and hash meets difficulty",
    validate_block(good_block) == 0
  );
  return NULL;
}

static char  *test_update_local_blockchain() {
  _fill_mempool();
  Block *good_block = mine_block();
  unsigned int prev_height = chain_height;
  update_local_blockchain(good_block);
  unsigned char block_hash[BLOCK_HASH_LEN];
  hash_blockheader(block_hash, &(good_block->header));
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
    blockchain_find(block_hash) == good_block
  );
  return NULL;
}

static char  *test_update_utxo_pool() {
  _fill_mempool();
  Block *good_block = mine_block();
  unsigned int prev_utxo_size = HASH_COUNT(utxo_pool);
  unsigned int prev_wallet_size = HASH_COUNT(wallet_pool);
  update_UTXO_pool_and_wallet_pool(good_block);
  mu_assert(
    "UTXO pool didn't change by expected amount",
    prev_utxo_size+1 == HASH_COUNT(utxo_pool)
  );
  mu_assert(
    "Wallet pool didn't change by expected amount",
    prev_wallet_size+1 == HASH_COUNT(wallet_pool)
  ); 
  return NULL;
}

// static char  *test_update_mempool() {
//   _fill_mempool();
//   Block *good_block = mine_block();
//   unsigned int prev_mempool_size = HASH_COUNT(mempool);
//   update_local_blockchain(good_block);
//   unsigned char block_hash[BLOCK_HASH_LEN];
//   hash_blockheader(block_hash, &(good_block->header));
//   mu_assert(
//     "Top Block Hash not set",
//     memcmp(top_block_header_hash, block_hash, BLOCK_HASH_LEN) == 0
//   );
//   return NULL;
// }


static char *all_tests() {
  blockchain_init(); // Set the prev Header Hash
  wallet_init();
  utxo_pool_init();
  mu_run_test(test_coinbase_tx);
  mu_run_test(test_validate_txs);
  mu_run_test(test_validate_prev_block);
  mu_run_test(test_validate_all_tx);
  mu_run_test(test_validate_whole_block);
  mu_run_test(test_update_local_blockchain);
  mu_run_test(test_update_utxo_pool);
  
  return NULL;
}

int main() {
  char *result = all_tests();
  if (result != NULL) {
    printf("%s\n", result);
  } else {
    printf("validate_block.c passing!\n");
  }
  printf("Tests run: %d\n", tests_run);

  return result != 0;
}
