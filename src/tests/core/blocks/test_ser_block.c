#include <stdio.h>
#include <string.h>

#include <setjmp.h>
#include <cmocka.h>

#include "ser_block.h"
#include "crypto.h"
#include "fixtures_block.h"

static void test_ser_blockheader_equal(void **state){
  BlockHeader *header, *desered_header;
  unsigned char *sered_header;
  ssize_t written_ser_header, read_ser_header;

  header = *state;
  sered_header = ser_blockheader_alloc(&written_ser_header, header);
  desered_header = deser_blockheader_alloc(&read_ser_header, sered_header);

  assert_int_equal(read_ser_header, BLOCKHEADER_SER_LEN);
  assert_int_equal(read_ser_header, written_ser_header);

  assert_int_equal(header->timestamp, desered_header->timestamp);
  assert_int_equal(header->nonce, desered_header->nonce);
  assert_memory_equal(
    header->prev_header_hash,
    desered_header->prev_header_hash,
    BLOCK_HASH_LEN
  );
  assert_memory_equal(
    header->all_tx,
    desered_header->all_tx,
    ALL_TX_HASH_LEN
  );

  free(sered_header);
  free(desered_header);
}

static void test_ser_blockheader_fill_buf(void **state) {
  BlockHeader *header;
  unsigned char *sered_header;
  unsigned char sered_fill_header[BLOCKHEADER_SER_LEN];

  header = *state;
  sered_header = ser_blockheader_alloc(NULL, header);

  // Ensure that we fill the entire buffer
  for (int i = 0; i < 5; i++) {
    memset(sered_fill_header, i, BLOCKHEADER_SER_LEN);
    ser_blockheader(sered_fill_header, header);
    assert_memory_equal(sered_header, sered_fill_header, BLOCKHEADER_SER_LEN);
  }

  free(sered_header);
}

static void test_ser_blockheader_pad_bytes(void **state) {
  BlockHeader *header, *pad_header;
  unsigned char *sered_header;
  unsigned char sered_pad_header[BLOCKHEADER_SER_LEN];

  header = *state;
  sered_header = ser_blockheader_alloc(NULL, header);

  // Ensure we don't serialize padding bytes
  pad_header = malloc(sizeof(BlockHeader));
  for (int i = 0; i < 5; i++) {
    memset(pad_header, i, sizeof(BlockHeader));
    pad_header->timestamp = header->timestamp;
    pad_header->nonce = header->nonce;
    memcpy(
      pad_header->prev_header_hash,
      header->prev_header_hash,
      BLOCK_HASH_LEN
    );
    memcpy(
      pad_header->all_tx,
      header->all_tx,
      ALL_TX_HASH_LEN
    );
    ser_blockheader(sered_pad_header, pad_header);
    assert_memory_equal(sered_header, sered_pad_header, BLOCKHEADER_SER_LEN);
  }

  free(pad_header);
  free(sered_header);
}

static void test_ser_block_equal(void **state) {
  Block *block, *desered_block;
  unsigned char *sered_block;
  unsigned char base_tx_hash[TX_HASH_LEN], deser_tx_hash[TX_HASH_LEN];
  ssize_t written_ser_block, read_ser_block;

  block = *state;
  sered_block = ser_block_alloc(&written_ser_block, block);
  desered_block = deser_block_alloc(&read_ser_block, sered_block);

  assert_int_equal(read_ser_block, (ssize_t)size_ser_block(block));
  assert_int_equal(read_ser_block, written_ser_block);

  assert_int_equal(block->num_txs, desered_block->num_txs);
  assert_int_equal(block->header.timestamp, desered_block->header.timestamp);
  assert_int_equal(block->header.nonce, desered_block->header.nonce);
  assert_memory_equal(
    block->header.prev_header_hash,
    desered_block->header.prev_header_hash,
    BLOCK_HASH_LEN
  );
  assert_memory_equal(
    block->header.all_tx,
    desered_block->header.all_tx,
    ALL_TX_HASH_LEN
  );

  for(unsigned int i = 0; i < block->num_txs; i++){
    hash_tx(base_tx_hash, block->txs[i]);
    hash_tx(deser_tx_hash, desered_block->txs[i]);
    assert_memory_equal(base_tx_hash, deser_tx_hash, TX_HASH_LEN);
  }

  free(sered_block);
  free(desered_block);
}

static void test_ser_block_fill_buf(void **state) {
  Block *block;
  unsigned char *sered_block, *sered_fill_block;
  size_t block_ser_size;

  block = *state;
  sered_block = ser_block_alloc(NULL, block);

  // Ensure that we fill the entire buffer
  block_ser_size = size_ser_block(block);
  sered_fill_block = malloc(block_ser_size);
  for (int i = 0; i < 5; i++) {
    memset(sered_fill_block, i, block_ser_size);
    ser_block(sered_fill_block, block);
    assert_memory_equal(sered_block, sered_fill_block, block_ser_size);
  }

  free(sered_block);
  free(sered_fill_block);
}

int main() {
  const struct CMUnitTest tests[] = {
    cmocka_unit_test_setup_teardown(
      test_ser_blockheader_equal,
      fixture_setup_unlinked_header,
      fixture_teardown_unlinked_header
    ),
    cmocka_unit_test_setup_teardown(
      test_ser_blockheader_fill_buf,
      fixture_setup_unlinked_header,
      fixture_teardown_unlinked_header
    ),
    cmocka_unit_test_setup_teardown(
      test_ser_blockheader_pad_bytes,
      fixture_setup_unlinked_header,
      fixture_teardown_unlinked_header
    ),
    cmocka_unit_test_setup_teardown(
      test_ser_block_equal,
      fixture_setup_unlinked_block,
      fixture_teardown_unlinked_block
    ),
    cmocka_unit_test_setup_teardown(
      test_ser_block_fill_buf,
      fixture_setup_unlinked_block,
      fixture_teardown_unlinked_block
    )
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
