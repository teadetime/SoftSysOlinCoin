#include <stdio.h>

#include <setjmp.h>
#include <cmocka.h>

#include "blockchain.h"
#include "init_db.h"
#include "fixtures_block.h"
#include "fixtures_global.h"

static void test_blockchain_init_exists(void **state) {
  (void)state;
  int init_ret = blockchain_init_leveldb(TEST_DB_LOC);
  assert_int_equal(init_ret, 0);
  destroy_blockchain();
}

static void test_blockchain_init_correct(void **state) {
  (void)state;
  Block genesis_block;
  unsigned char genesis_hash[BLOCK_HASH_LEN];
  unsigned int count;

  blockchain_count(&count);
  assert_int_equal(count, 1);

  genesis_block.num_txs = 0;
  genesis_block.txs = NULL;
  genesis_block.header.timestamp = 0;
  genesis_block.header.nonce = 0;
  memset(genesis_block.header.all_tx, 0, TX_HASH_LEN);
  memset(genesis_block.header.prev_header_hash, 0, BLOCK_HASH_LEN);
  hash_blockheader(genesis_hash, &genesis_block.header);

  assert_int_equal(chain_height, 1);
  assert_memory_equal(top_block_header_hash, genesis_hash, BLOCK_HASH_LEN);
}

static void test_blockchain_add(void **state) {
  Block *block;
  block = *state;
  int ret_add = blockchain_add_leveldb(block);
  unsigned int entries;
  blockchain_count(&entries);
  assert_int_equal(ret_add, 0);
  assert_int_equal(entries, 2);
}

static void test_blockchain_find(void **state) {
  Block *block, *ret_block;
  unsigned char hash[BLOCK_HASH_LEN];

  block = *state;
  hash_blockheader(hash, &(block->header));

  blockchain_add_leveldb(block);
  int ret_find = blockchain_find_leveldb(&ret_block, hash);
  assert_int_equal(ret_find, 0);

  unsigned char found_hash[BLOCK_HASH_LEN];
  hash_blockheader(found_hash, &(ret_block->header));
  assert_memory_equal(hash, found_hash, BLOCK_HASH_LEN);

  free_block(ret_block);
}

static void test_blockchain_remove(void **state) {
  Block *block, *ret_block = NULL;
  unsigned char hash[BLOCK_HASH_LEN];
  unsigned int prev_count, count;

  block = *state;
  hash_blockheader(hash, &(block->header));

  blockchain_add_leveldb(block);
  blockchain_count(&prev_count);

  int ret_remove = blockchain_remove_leveldb(hash);
  blockchain_count(&count);
  assert_int_equal(ret_remove, 0);
  assert_int_equal(count, prev_count - 1);

  int ret_find = blockchain_find_leveldb(&ret_block, hash);
  assert_int_not_equal(ret_find, 0);
  assert_ptr_equal(ret_block, NULL);
}

int main() {
  int (*blockchain_setup[])(void**) = {
    fixture_setup_blockchain,
    fixture_setup_unlinked_block,
    NULL
  };
  int (*blockchain_teardown[])(void**) = {
    fixture_teardown_blockchain,
    fixture_teardown_unlinked_block,
    NULL
  };
  ComposedFixture composition;

  composition.setup = blockchain_setup;
  composition.teardown = blockchain_teardown;

  const struct CMUnitTest tests[] = {
    cmocka_unit_test_prestate_setup_teardown(
      test_blockchain_init_exists,
      NULL,
      NULL,
      NULL
    ),
    cmocka_unit_test_prestate_setup_teardown(
      test_blockchain_init_correct,
      fixture_setup_blockchain,
      fixture_teardown_blockchain,
      NULL
    ),
    cmocka_unit_test_prestate_setup_teardown(
      test_blockchain_add,
      fixture_setup_compose,
      fixture_teardown_compose,
      &composition
    ),
    cmocka_unit_test_prestate_setup_teardown(
      test_blockchain_find,
      fixture_setup_compose,
      fixture_teardown_compose,
      &composition
    ),
    cmocka_unit_test_prestate_setup_teardown(
      test_blockchain_remove,
      fixture_setup_compose,
      fixture_teardown_compose,
      &composition
    )
  };

  return cmocka_run_group_tests(tests, fixture_clear_blockchain, NULL);
}
