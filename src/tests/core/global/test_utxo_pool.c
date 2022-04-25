#include <stdio.h>

#include <setjmp.h>
#include <cmocka.h>

#include "utxo_pool.h"
#include "crypto.h"
#include "init_db.h"
#include "fixtures_tx.h"
#include "fixtures_global.h"

static void test_utxo_pool_init(void **state) {
  (void)state;
  assert_ptr_not_equal(utxo_pool_db, NULL);
  assert_ptr_not_equal(utxo_pool_path, NULL);
}

static void test_utxo_pool_add(void **state) {
  Transaction *tx;
  unsigned int num_utxo_prev, num_utxo_after;
  int ret;

  tx = *state;
  utxo_pool_count(&num_utxo_prev);
  ret = utxo_pool_add_leveldb(tx,0);
  utxo_pool_count(&num_utxo_after);

  assert_int_equal(ret, 0);
  assert_int_equal(num_utxo_after, num_utxo_prev+1);
}

static void test_utxo_pool_find(void **state) {
  Transaction *tx;
  UTXO *ret_utxo = NULL;
  unsigned char hash[TX_HASH_LEN];
  int ret_found;

  tx = *state;
  hash_tx(hash, tx);

  utxo_pool_add_leveldb(tx, 0);
  ret_found = utxo_pool_find_leveldb(&ret_utxo, hash, 0);

  assert_int_equal(ret_found, 0);
  assert_ptr_not_equal(ret_utxo, NULL);
  assert_int_equal(ret_utxo->amt, tx->outputs[0].amt);
  assert_memory_equal(
    ret_utxo->public_key_hash,
    tx->outputs[0].public_key_hash,
    PUB_KEY_HASH_LEN
  );
}

static void test_utxo_pool_remove(void **state) {
  Transaction *tx;
  UTXO  *ret_utxo = NULL;
  unsigned char hash[TX_HASH_LEN];
  int ret_remove, ret_found;

  tx = *state;
  hash_tx(hash, tx);

  utxo_pool_add_leveldb(tx, 0);
  ret_remove = utxo_pool_remove_leveldb(hash, 0);

  assert_int_equal(ret_remove, 0);
  assert_int_equal(ret_utxo, NULL);

  ret_found = utxo_pool_find_leveldb(&ret_utxo, hash, 0);
  assert_int_not_equal(ret_found, 0);
}

int main() {
  int (*utxo_tx_setup[])(void**) = {
    fixture_setup_utxo_pool,
    fixture_setup_unlinked_tx,
    NULL
  };
  int (*utxo_tx_teardown[])(void**) = {
    fixture_teardown_utxo_pool,
    fixture_teardown_unlinked_tx,
    NULL
  };
  ComposedFixture composition;

  composition.setup = utxo_tx_setup;
  composition.teardown = utxo_tx_teardown;

  const struct CMUnitTest tests[] = {
    cmocka_unit_test_setup_teardown(
      test_utxo_pool_init,
      fixture_setup_utxo_pool,
      fixture_teardown_utxo_pool
    ),
    cmocka_unit_test_prestate_setup_teardown(
      test_utxo_pool_add,
      fixture_setup_compose,
      fixture_teardown_compose,
      &composition
    ),
    cmocka_unit_test_prestate_setup_teardown(
      test_utxo_pool_find,
      fixture_setup_compose,
      fixture_teardown_compose,
      &composition
    ),
    cmocka_unit_test_prestate_setup_teardown(
      test_utxo_pool_remove,
      fixture_setup_compose,
      fixture_teardown_compose,
      &composition
    )
  };

  return cmocka_run_group_tests(tests, fixture_clear_utxo_pool, NULL);
}
