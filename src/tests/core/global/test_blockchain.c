#include <stdio.h>
#include "minunit.h"
#include "blockchain.h"

int tests_run = 0;

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
  block->txs = tx;

  block->header.timestamp = 1;
  memset(block->header.all_tx, 1, TX_HASH_LEN);
  memset(block->header.prev_header_hash, 1, BLOCK_HASH_LEN);
  block->header.nonce = 1;

  return block;
}

static char *test_blockchain_init_exists() {
  blockchain = NULL;
  blockchain_init();
  mu_assert(
      "Genesis block was not created",
      blockchain != NULL
  );
  return NULL;
}

static char *test_blockchain_init_correct() {
  char empty_tx_hash[TX_HASH_LEN];
  char empty_block_hash[BLOCK_HASH_LEN];

  memset(empty_tx_hash, 0, TX_HASH_LEN);
  memset(empty_block_hash, 0, BLOCK_HASH_LEN);

  blockchain_init();
  mu_assert(
    "Chain Height Incorrect",
    chain_height == 1
  );
  mu_assert(
    "Genesis num_txs incorrect",
    blockchain->block->num_txs == 0
  );
  mu_assert(
    "Genesis txs incorrect",
    blockchain->block->txs == NULL
  );
  mu_assert(
    "Genesis timestamp incorrect",
    blockchain->block->header.timestamp == 0
  );
  mu_assert(
    "Genesis nonce incorrect",
    blockchain->block->header.nonce == 0
  );
  mu_assert(
    "Genesis tx hash incorrect",
    memcmp(blockchain->block->header.all_tx, empty_tx_hash, TX_HASH_LEN) == 0
  );
  mu_assert(
    "Genesis prev block hash incorrect",
    memcmp(
      blockchain->block->header.prev_header_hash,
      empty_block_hash,
      BLOCK_HASH_LEN
    ) == 0
  );
  return NULL;
}

static char  *test_blockchain_add() {
  Block *block, *ret_block;

  block = _make_block();
  blockchain_init();
  ret_block = blockchain_add(block);
  mu_assert(
    "Add did not return correct block",
    ret_block == block
  );

  free(block->txs);
  free(block);

  return NULL;
}

static char  *test_blockchain_find() {
  Block *block, *ret_block;
  unsigned char hash[BLOCK_HASH_LEN];

  block = _make_block();
  hash_blockheader(hash, &(block->header));

  blockchain_init();
  blockchain_add(block);
  ret_block = blockchain_find(hash);
  mu_assert(
    "Find did not return correct block",
    ret_block == block
  );

  free(block->txs);
  free(block);

  return NULL;
}

static char  *test_blockchain_remove() {
  Block *block, *ret_block;
  unsigned char hash[BLOCK_HASH_LEN];

  block = _make_block();
  hash_blockheader(hash, &(block->header));

  blockchain_init();
  blockchain_add(block);
  unsigned long prev_chain_height = chain_height;
  ret_block = blockchain_remove(hash);

  mu_assert(
    "Remove did not return correct block",
    ret_block == block
  );
  mu_assert(
    "Chain Height not decremented",
    chain_height = prev_chain_height-1
  );
  
  ret_block = blockchain_find(hash);
  mu_assert(
    "Block was not removed",
    ret_block == NULL
  );

  free(block->txs);
  free(block);

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
  char *result = all_tests();
  if (result != NULL) {
    printf("%s\n", result);
  } else {
    printf("blockchain.c passing!\n");
  }
  printf("Tests run: %d\n", tests_run);

  return result != 0;
}
