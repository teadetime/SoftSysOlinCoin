#include <stdio.h>
#include "minunit.h"
#include "mempool.h"
#include "utxo_pool.h"
#include "create_block.h"
#include "time.h"
#include "ser_block.h"
#include "crypto.h"
#include "wallet_pool.h"

int tests_run = 0;

static void _set_header(BlockHeader *header) {
  header->timestamp = 100;
  header->nonce = 123;
  memset(header->prev_header_hash, 0xFA, BLOCK_HASH_LEN);
  memset(header->all_tx, 0xAF, ALL_TX_HASH_LEN);
}

static Transaction *_make_tx() {
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

static void _fill_mempool(){
  Transaction *input_tx, *tx1;
  input_tx = _make_tx();
  input_tx->outputs[0].amt = 100;
  utxo_pool_init_leveldb();

  utxo_pool_add_leveldb(input_tx, 0);

  tx1 = _make_tx();
  hash_tx(tx1->inputs[0].prev_tx_id, input_tx);
  tx1->inputs[0].prev_utxo_output = 0;
  tx1->outputs[0].amt = 90;
  mempool_init();
  mempool_add(tx1);
}

static char *test_ser_blockheader(){
  BlockHeader *header, *desered_header;
  unsigned char *sered_header, *sered_header_2;
  ssize_t written_ser_header, read_ser_header;

  header = malloc(sizeof(BlockHeader));
  _set_header(header);
  sered_header = ser_blockheader_alloc(&written_ser_header, header);
  desered_header = deser_blockheader_alloc(&read_ser_header, sered_header);

  mu_assert(
    "Num of bytes read incorrect",
    read_ser_header == (ssize_t)size_ser_blockheader()
  );
  mu_assert(
    "Num of bytes read and written don't match up",
    read_ser_header == written_ser_header
  );

  mu_assert(
    "Timestamp isn't consistent after serialization",
    header->timestamp == desered_header->timestamp
  );
  mu_assert(
    "Nonce isn't consistent after serialization",
    header->nonce == desered_header->nonce
  );
  mu_assert(
    "Prev hash isn't consistent after serialization",
    memcmp(
      header->prev_header_hash,
      desered_header->prev_header_hash,
      BLOCK_HASH_LEN
    ) == 0
  );
  mu_assert(
    "All tx hash isn't consistent after serialization",
    memcmp(
      header->all_tx,
      desered_header->all_tx,
      ALL_TX_HASH_LEN
    ) == 0
  );

  // Ensure that we don't have padding bytes
  for (int i = 0; i < 10; i++) {
    sered_header_2 = ser_blockheader_alloc(NULL, header);
    mu_assert(
      "Serialization of header isn't consistent",
      memcmp(sered_header, sered_header_2, size_ser_blockheader())  == 0
    );
    free(sered_header_2);
  }

  free(header);
  free(sered_header);
  free(desered_header);

  return NULL;
}

static char *test_ser_block(){
  Block *block, *desered_block;
  unsigned char *sered_block, *sered_block_2;
  ssize_t written_ser_block, read_ser_block;

  _fill_mempool();
  wallet_init_leveldb();
  block = create_block_alloc();
  sered_block = ser_block_alloc(&written_ser_block, block);
  desered_block = deser_block_alloc(&read_ser_block, sered_block);

  mu_assert(
    "Num of bytes read incorrect",
    read_ser_block == (ssize_t)size_ser_block(block)
  );
  mu_assert(
    "Num of bytes read and written don't match up",
    read_ser_block == written_ser_block
  );

  mu_assert(
    "Num Txs don't match",
    block->num_txs == desered_block->num_txs
  );
  mu_assert(
    "Header timestamp isn't consistent after serialization",
    block->header.timestamp == desered_block->header.timestamp
  );
  mu_assert(
    "Header nonce isn't consistent after serialization",
    block->header.nonce == desered_block->header.nonce
  );
  mu_assert(
    "Header prev hash isn't consistent after serialization",
    memcmp(
      block->header.prev_header_hash,
      desered_block->header.prev_header_hash,
      BLOCK_HASH_LEN
    ) == 0
  );
  mu_assert(
    "Header all tx hash isn't consistent after serialization",
    memcmp(
      block->header.all_tx,
      desered_block->header.all_tx,
      ALL_TX_HASH_LEN
    ) == 0
  );

  unsigned char base_tx_hash[TX_HASH_LEN];
  unsigned char deser_tx_hash[TX_HASH_LEN];
  for(unsigned int i = 0; i < block->num_txs; i++){
    hash_tx(base_tx_hash, block->txs[i]);
    hash_tx(deser_tx_hash, desered_block->txs[i]);
    mu_assert(
      "Transaction hashes don't match -> Transaction serialization error",
      memcmp(base_tx_hash, deser_tx_hash, TX_HASH_LEN) == 0
    );
  }

  // Ensure that we don't have padding bytes
  for (int i = 0; i < 10; i++) {
    sered_block_2 = ser_block_alloc(NULL, block);
    mu_assert(
      "Serialization of block isn't consistent",
      memcmp(sered_block, sered_block_2, size_ser_block(block)) == 0
    );
    free(sered_block_2);
  }

  free(block);
  free(sered_block);
  free(desered_block);
  destroy_db(&utxo_pool_db, utxo_pool_path);
  destroy_wallet();
  return NULL;
}

static char *all_tests() {
  mu_run_test(test_ser_blockheader);
  mu_run_test(test_ser_block);
  return NULL;
}

int main() {
  char *result = all_tests();
  if (result != NULL) {
    printf("%s\n", result);
  } else {
    printf("ser_block.c passing!\n");
  }
  printf("Tests run: %d\n", tests_run);

  return result != 0;
}
