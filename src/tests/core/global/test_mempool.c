#include <stdio.h>

#include <setjmp.h>
#include <cmocka.h>

#include "mempool.h"
#include "crypto.h"
#include "fixtures_tx.h"
#include "fixtures_global.h"

static void test_mempool_init(void **state) {
  (void)state;
  mempool_init();
  assert_ptr_equal(mempool, NULL);
}

static void test_mempool_add(void **state) {
  Transaction *tx, *ret_tx;
  unsigned int prev_count, count;
  unsigned char tx_hash[TX_HASH_LEN], ret_hash[TX_HASH_LEN];

  tx = *state;
  prev_count = HASH_COUNT(mempool);
  ret_tx = mempool_add(tx);
  count = HASH_COUNT(mempool);
  hash_tx(tx_hash, tx);
  hash_tx(ret_hash, ret_tx);

  assert_ptr_not_equal(ret_tx, NULL);
  assert_int_equal(count, prev_count + 1);
  assert_memory_equal(tx_hash, ret_hash, TX_HASH_LEN);
}

static void test_mempool_find(void **state) {
  Transaction *tx, *ret_tx;
  unsigned char tx_hash[TX_HASH_LEN], ret_hash[TX_HASH_LEN];

  tx = *state;
  mempool_add(tx);
  hash_tx(tx_hash, tx);

  ret_tx = mempool_find(tx_hash);
  hash_tx(ret_hash, ret_tx);

  assert_ptr_not_equal(ret_tx, NULL);
  assert_memory_equal(tx_hash, ret_hash, TX_HASH_LEN);
}

static void test_mempool_remove(void **state) {
  Transaction *tx, *ret_tx;
  unsigned char hash[TX_HASH_LEN];
  unsigned int prev_count, count;

  tx = *state;
  hash_tx(hash, tx);

  mempool_add(tx);
  prev_count = HASH_COUNT(mempool);

  ret_tx = mempool_remove(hash);
  count = HASH_COUNT(mempool);
  assert_ptr_not_equal(ret_tx, NULL);
  assert_int_equal(count, prev_count - 1);

  ret_tx = mempool_find(hash);
  assert_ptr_equal(ret_tx, NULL);

  free_tx(ret_tx);
}

int main() {
  int (*mempool_setup[])(void**) = {
    fixture_setup_mempool,
    fixture_setup_unlinked_tx,
    NULL
  };
  int (*mempool_teardown[])(void**) = {
    fixture_teardown_mempool,
    fixture_teardown_unlinked_tx,
    NULL
  };
  ComposedFixture composition;

  composition.setup = mempool_setup;
  composition.teardown = mempool_teardown;

  const struct CMUnitTest tests[] = {
    cmocka_unit_test_prestate_setup_teardown(
      test_mempool_init,
      fixture_setup_mempool,
      fixture_teardown_mempool,
      NULL
    ),
    cmocka_unit_test_prestate_setup_teardown(
      test_mempool_add,
      fixture_setup_compose,
      fixture_teardown_compose,
      &composition
    ),
    cmocka_unit_test_prestate_setup_teardown(
      test_mempool_find,
      fixture_setup_compose,
      fixture_teardown_compose,
      &composition
    ),
    cmocka_unit_test_prestate_setup_teardown(
      test_mempool_remove,
      fixture_setup_compose,
      fixture_teardown_compose,
      &composition
    )
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
