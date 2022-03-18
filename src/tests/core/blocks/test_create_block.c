#include <stdio.h>
#include "minunit.h"
#include "mempool.h"
#include "utxo_pool.h"
#include "create_block.h"

int tests_run = 0;

Transaction *_make_tx() {
  Transaction *tx;
  Input *in;
  Output *out;

  tx = malloc(sizeof(Transaction));
  in = malloc(sizeof(Input));
  out = malloc(sizeof(Output));

  memset(in, 0, sizeof(Input));
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

static char *test_create_coinbase_tx(){
  unsigned long test = 10;
  Transaction *tx = create_coinbase_tx(test);
  mu_assert(
    "Coinbase Inputs are not NULL",
    tx->inputs == NULL
  );
  mu_assert(
    "Coinbase num Inputs is not 0",
    tx->num_inputs == 0
  );
  mu_assert(
    "Coinbase num Outputs is not 1",
    tx->num_outputs == 1
  );
  mu_assert(
    "Coinbase output amount is not what is expected",
    tx->outputs[0].amt == test+BLOCK_REWARD
  );
  return NULL;
}

static char *test_get_txs_from_mempool(){
  _fill_mempool();
  Transaction **test_ptr = malloc(sizeof(Transaction**));
  unsigned int num_tx = get_txs_from_mempool(&test_ptr);
  mu_assert(
    "Wrong number of transactions",
    num_tx == 2
  );
  mu_assert(
    "Coinbase TX num_input doesn't match",
    test_ptr[0]->num_inputs == 0
  );
  mu_assert(
    "Coinbase TX num_outputs doesn't match",
    test_ptr[0]->num_outputs == 1
  );
  mu_assert(
    "Coinbase TX output amt doesn't match",
    test_ptr[0]->outputs[0].amt == 10+BLOCK_REWARD
  );
  mu_assert(
    "TX num_input doesn't match",
    test_ptr[1]->num_inputs == 1
  );
  mu_assert(
    "TX num_outputs doesn't match",
    test_ptr[1]->num_outputs == 1
  );
  mu_assert(
    "TX output amt doesn't match",
    test_ptr[1]->outputs[0].amt == 90
  );
  unsigned char *total_hash = hash_all_tx(test_ptr,2);
  unsigned char *total_hash2 = hash_all_tx(test_ptr,2);
  mu_assert(
    "Total hash is inconsistent",
    memcmp(total_hash, total_hash2, ALL_TX_HASH_LEN) == 0
  );

  BlockHeader *test_header = create_block_header(test_ptr, 2);
  mu_assert(
    "Total hash is not consistent in header creation",
    memcmp(total_hash, test_header->all_tx, ALL_TX_HASH_LEN) == 0
  );
  mu_assert(
    "Header nonce invalid",
    test_header->nonce == 0
  );
  mu_assert(
    "Header timestamp invalid",
    test_header->timestamp > 1647573975
  );


  Block *test_block = create_block_alloc();
  mu_assert(
    "Block txs coinbase is bad",
    test_block->txs[0]->num_inputs !=1
  );
  mu_assert(
    "TX output amt doesn't match",
    test_block->txs[1]->outputs[0].amt == 90
  );
  change_nonce(test_block);
  mu_assert(
    "Nonce increment failed",
    test_block->header.nonce == 1
  );
  unsigned char *header_hash = hash_header(&(test_block->header));
  unsigned char *header_hash2 = hash_header(&(test_block->header));
  mu_assert(
    "Header hash is inconsistent",
    memcmp(header_hash, header_hash2, BLOCK_HASH_LEN) == 0
  );
  mu_assert(
    "Header hash failed",
    try_header_hash(&(test_block->header)) == 1
  );
  mu_assert(
    "Block hash failed",
    mine_block(test_block) == 1
  );

  while(mine_block(test_block) != 0){
    change_nonce(test_block);
  }
  print_block_header(&(test_block->header), "");
  dump_buf("", "MINED TX: ", hash_header(&(test_block->header)), BLOCK_HASH_LEN);
  free(test_block);
  free(test_header);
  free(total_hash);
  free(total_hash2);
  free(header_hash);
  free(header_hash2);
  free(test_ptr);
  return NULL;
}

static char  *test_calc_block_reward() {
  mu_assert(
    "calc clock reward not returning correct rewards",
    calc_block_reward(0) == BLOCK_REWARD
  );
  return NULL;
}
static char  *test_calc_num_tx_target() {
  mu_assert(
    "calc_num_tx_target not returning correct target",
    calc_num_tx_target() == DESIRED_NUM_TX
  );
  return NULL;
}
static char  *test_get_difficulty() {
  mu_assert(
    "difficulty doesn't match constant",
    get_difficulty() == HASH_DIFFICULTY
  );
  return NULL;
}



static char *all_tests() {
  mu_run_test(test_calc_block_reward);
  mu_run_test(test_get_txs_from_mempool);
  mu_run_test(test_calc_num_tx_target);
  mu_run_test(test_get_difficulty);
  mu_run_test(test_create_coinbase_tx);
  return NULL;
}

int main() {
  char *result = all_tests();
  if (result != NULL) {
    printf("%s\n", result);
  } else {
    printf("create_block.c passing!\n");
  }
  printf("Tests run: %d\n", tests_run);

  return result != 0;
}
