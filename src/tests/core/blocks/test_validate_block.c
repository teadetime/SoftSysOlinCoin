#include <stdlib.h>
#include "minunit.h"
#include "validate_block.h"
#include "create_block.h"
#include "mempool.h"
#include "utxo_pool.h"
#include "blockchain.h"
#include "crypto.h"
#include "wallet_pool.h"

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
  utxo_pool_init_leveldb(TEST_DB_LOC);
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

static char  *test_coinbase_tx() {
  blockchain_init_leveldb(TEST_DB_LOC);
  wallet_init_leveldb(TEST_DB_LOC);
  _fill_mempool();
  Block *test_block = create_block_alloc(); // Note this creates a valid block

  mu_assert(
    "Coinbase Validation Broken",
    validate_coinbase_tx(test_block->txs, test_block->num_txs) == 0
  );
  destroy_db(&utxo_pool_db, utxo_pool_path);
  destroy_blockchain();
  destroy_wallet();
  return NULL;
}

static char  *test_validate_txs() {
  blockchain_init_leveldb(TEST_DB_LOC);
  wallet_init_leveldb(TEST_DB_LOC);
  _fill_mempool();
  Block *test_block = create_block_alloc();

  mu_assert(
    "Transaction validation failing on valid txs",
    validate_incoming_block_txs(test_block->txs, test_block->num_txs) == 0
  );
  destroy_db(&utxo_pool_db, utxo_pool_path);
  destroy_blockchain();
  destroy_wallet();
  return NULL;
}


static char  *test_validate_prev_block() {
  blockchain_init_leveldb(TEST_DB_LOC);
  wallet_init_leveldb(TEST_DB_LOC);
  _fill_mempool();
  Block *test_block = create_block_alloc();
  mu_assert(
    "Previous block hash compare error",
    validate_prev_block_exists(test_block) == 0
  );
  destroy_db(&utxo_pool_db, utxo_pool_path);
  destroy_blockchain();
  destroy_wallet();
  return NULL;
}

static char  *test_validate_all_tx() {
  blockchain_init_leveldb(TEST_DB_LOC);
  wallet_init_leveldb(TEST_DB_LOC);
  _fill_mempool();
  Block *test_block = create_block_alloc();
  mu_assert(
    "Failed Validation of good all_tx_hash",
    validate_all_tx_hash(test_block) == 0
  );
  destroy_db(&utxo_pool_db, utxo_pool_path);
  destroy_blockchain();
  destroy_wallet();
  return NULL;
}

static char  *test_validate_block_double_spend() {
  blockchain_init_leveldb(TEST_DB_LOC);
  _fill_mempool();
  wallet_init_leveldb(TEST_DB_LOC);
  Block *test_block = create_block_alloc();
  mu_assert(
    "Detecting double spend in valid block",
    validate_block_double_spend(test_block) == 0
  );
  destroy_db(&utxo_pool_db, utxo_pool_path);
  destroy_blockchain();
  destroy_wallet();
  return NULL;
}

static char  *test_validate_whole_block() {
  blockchain_init_leveldb(TEST_DB_LOC);
  wallet_init_leveldb(TEST_DB_LOC);
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
  destroy_db(&utxo_pool_db, utxo_pool_path);
  destroy_blockchain();
  destroy_wallet();
  return NULL;
}

static char *all_tests() {
  mu_run_test(test_coinbase_tx);
  mu_run_test(test_validate_txs);
  mu_run_test(test_validate_prev_block);
  mu_run_test(test_validate_block_double_spend);
  mu_run_test(test_validate_all_tx);
  mu_run_test(test_validate_whole_block);

  return NULL;
}

int main() {
  create_proj_folders();
  char *result = all_tests();
  if (result != NULL) {
    printf("%s\n", result);
  } else {
    printf("validate_block.c passing!\n");
  }
  printf("Tests run: %d\n", tests_run);

  return result != 0;
}
