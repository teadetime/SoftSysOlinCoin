#include <stdio.h>
#include "minunit.h"
#include "blockchain.h"
#include "init_db.h"

int tests_run = 0;

Block *_make_block() {
  Block *block;
  Transaction *tx;

  block = malloc(sizeof(Block));

  block->txs = malloc(sizeof(Transaction*) * 1);
  block->txs[0] = malloc(sizeof(Transaction));
  tx = block->txs[0];

  tx->num_inputs = 0;
  tx->inputs = NULL;
  tx->num_outputs = 0;
  tx->outputs = NULL;

  block->num_txs = 1;

  block->header.timestamp = 1;
  memset(block->header.all_tx, 1, TX_HASH_LEN);
  memset(block->header.prev_header_hash, 1, BLOCK_HASH_LEN);
  block->header.nonce = 1;

  return block;
}

static char *test_blockchain_init_exists() {
  int init_ret = blockchain_init_leveldb(TEST_DB_LOC);
  destroy_blockchain();
  init_ret = blockchain_init_leveldb(TEST_DB_LOC);
  mu_assert(
      "Init return indicates fialure",
      init_ret == 0
  );
  unsigned int count;
  blockchain_count(&count);
  mu_assert(
      "Genesis block was not created",
      count == 1
  );
  destroy_blockchain();
  return NULL;
}

static char *test_blockchain_init_correct() {
  char empty_tx_hash[TX_HASH_LEN];
  char empty_block_hash[BLOCK_HASH_LEN];

  memset(empty_tx_hash, 0, TX_HASH_LEN);
  memset(empty_block_hash, 0, BLOCK_HASH_LEN);

  blockchain_init_leveldb(TEST_DB_LOC);
  
  mu_assert(
    "Chain Height Incorrect",
    chain_height == 1
  );

  // The following tests require to find the genesis block
  // mu_assert(
  //   "Genesis num_txs incorrect",
  //   blockchain->block->num_txs == 0
  // );
  // mu_assert(
  //   "Genesis txs incorrect",
  //   blockchain->block->txs == NULL
  // );
  // mu_assert(
  //   "Genesis timestamp incorrect",
  //   blockchain->block->header.timestamp == 0
  // );
  // mu_assert(
  //   "Genesis nonce incorrect",
  //   blockchain->block->header.nonce == 0
  // );
  // mu_assert(
  //   "Genesis tx hash incorrect",
  //   memcmp(blockchain->block->header.all_tx, empty_tx_hash, TX_HASH_LEN) == 0
  // );
  // mu_assert(
  //   "Genesis prev block hash incorrect",
  //   memcmp(
  //     blockchain->block->header.prev_header_hash,
  //     empty_block_hash,
  //     BLOCK_HASH_LEN
  //   ) == 0
  // );
  destroy_blockchain();
  return NULL;
}

static char  *test_blockchain_add() {

  blockchain_init_leveldb(TEST_DB_LOC);
  Block *block;
  block = _make_block();
  int ret_add = blockchain_add_leveldb(block);
  unsigned int entries;
  blockchain_count(&entries);
  mu_assert(
    "Add return code indicates failure",
    ret_add == 0
  );
  mu_assert(
    "Add did not return correct block",
    entries == 2
  );
  free(block->txs[0]);
  free(block->txs);
  free(block);
  destroy_blockchain();
  return NULL;
}

static char  *test_blockchain_find() {
  Block *block, *ret_block;
  unsigned char hash[BLOCK_HASH_LEN];

  block = _make_block();
  hash_blockheader(hash, &(block->header));

  blockchain_init_leveldb(TEST_DB_LOC);
  blockchain_add_leveldb(block);
  int ret_find = blockchain_find_leveldb(&ret_block, hash);
  mu_assert(
    "Function returned failure",
    ret_find == 0
  );
  unsigned char found_hash[BLOCK_HASH_LEN];
  hash_blockheader(found_hash, &(ret_block->header));
  mu_assert(
    "Find did not return correct block",
    memcmp(hash, found_hash, BLOCK_HASH_LEN) == 0
  );

  free(block->txs[0]);
  free(block->txs);
  free(block);
  destroy_blockchain();
  return NULL;
}

static char  *test_blockchain_remove() {
  Block *block, *ret_block = NULL;
  unsigned char hash[BLOCK_HASH_LEN];

  block = _make_block();
  hash_blockheader(hash, &(block->header));

  blockchain_init_leveldb(TEST_DB_LOC);
  blockchain_add_leveldb(block);
  unsigned long prev_chain_height = chain_height;
  unsigned int prev_count;
  blockchain_count(&prev_count);
  int ret_remove = blockchain_remove_leveldb(hash);
  unsigned int count;
  blockchain_count(&count);
  mu_assert(
    "Remove block returned failure code",
    ret_remove == 0
  );
  mu_assert(
    "Remove did not decrease blockchain size in expected amount",
    count == prev_count-1
  );
  mu_assert(
    "Chain Height not decremented",
    chain_height = prev_chain_height-1
  );

  int ret_find = blockchain_find_leveldb(&ret_block, hash);
  mu_assert(
    "Block Find returns success after block removed",
    ret_find != 0
  );
  mu_assert(
    "Block was not removed",
    ret_block == NULL
  );

  free(block->txs[0]);
  free(block->txs);
  free(block);
  free(ret_block);
  destroy_blockchain();
  return NULL;
}

static char *all_tests() {
  mu_run_test(test_blockchain_init_exists);
  mu_run_test(test_blockchain_init_correct);
  mu_run_test(test_blockchain_add);
  mu_run_test(test_blockchain_find);
  mu_run_test(test_blockchain_remove);
  return NULL;
}

int main() {
  create_proj_folders();
  char *result = all_tests();
  if (result != NULL) {
    printf("%s\n", result);
  } else {
    printf("blockchain.c passing!\n");
  }
  printf("Tests run: %d\n", tests_run);

  return result != 0;
}
