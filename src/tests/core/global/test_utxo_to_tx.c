#include <stdio.h>

#include <setjmp.h>
#include <cmocka.h>

#include "utxo_to_tx.h"
#include "crypto.h"
#include "fixtures_tx.h"
#include "fixtures_global.h"

static void test_utxo_to_tx_init(void **state) {
  (void)state;
  utxo_to_tx_init();
  assert_ptr_equal(utxo_to_tx, NULL);
}

static void test_utxo_to_tx_add(void **state) {
  Transaction *tx;
  int ret_val;
  Input *in;
  unsigned int prev_count, count;
  unsigned char tx_hash[TX_HASH_LEN];

  tx = *state;
  in = &tx->inputs[0];
  hash_tx(tx_hash, tx);
  prev_count = HASH_COUNT(utxo_to_tx);
  ret_val = utxo_to_tx_add(in->prev_tx_id, in->prev_utxo_output, tx_hash);
  count = HASH_COUNT(utxo_to_tx);
  assert_int_equal(ret_val, 0);

  // Relies on UTHash storing new additions as first value in utxo_to_tx global
  // variable, which *should* always happen since it starts as NULL
  assert_memory_equal(utxo_to_tx->id.tx_hash, in->prev_tx_id, TX_HASH_LEN);
  assert_int_equal(utxo_to_tx->id.vout, in->prev_utxo_output);
  assert_memory_equal(utxo_to_tx->tx_hash, tx_hash, TX_HASH_LEN);
  assert_int_equal(count, prev_count + 1);
}

static void test_utxo_to_tx_add_tx(void **state) {
  Transaction *tx;
  int ret_val, count;
  unsigned char tx_hash[TX_HASH_LEN];

  tx = *state;
  hash_tx(tx_hash, tx);
  ret_val = utxo_to_tx_add_tx(tx);
  assert_int_equal(ret_val, 0);

  // add_tx() calls add() under the hood, so we just test to ensure something
  // was added, and not what was added. This is better served by a mock
  count = HASH_COUNT(utxo_to_tx);
  assert_int_equal(count, 4);
}

static void test_utxo_to_tx_find(void **state) {
  Transaction *tx;
  Input *in;
  int ret_val;
  unsigned char tx_hash[TX_HASH_LEN], ret_hash[TX_HASH_LEN];

  tx = *state;
  in = &tx->inputs[0];
  hash_tx(tx_hash, tx);

  utxo_to_tx_add_tx(tx);
  ret_val = utxo_to_tx_find(ret_hash, in->prev_tx_id, in->prev_utxo_output);

  assert_int_equal(ret_val, 0);
  assert_memory_equal(tx_hash, ret_hash, TX_HASH_LEN);
}

static void test_utxo_to_tx_remove(void **state) {
  Transaction *tx;
  Input *in;
  int ret_val;
  unsigned int prev_count, count;

  tx = *state;
  in = &tx->inputs[0];
  utxo_to_tx_add_tx(tx);
  prev_count = HASH_COUNT(utxo_to_tx);

  ret_val = utxo_to_tx_remove(in->prev_tx_id, in->prev_utxo_output);
  count = HASH_COUNT(utxo_to_tx);
  assert_int_equal(ret_val, 0);
  assert_int_equal(count, prev_count - 1);

  ret_val = utxo_to_tx_find(NULL, in->prev_tx_id, in->prev_utxo_output);
  assert_int_equal(ret_val, 1);
}

int main() {
  int (*utxo_to_tx_setup[])(void**) = {
    fixture_setup_utxo_to_tx,
    fixture_setup_unlinked_tx,
    NULL
  };
  int (*utxo_to_tx_teardown[])(void**) = {
    fixture_teardown_utxo_to_tx,
    fixture_teardown_unlinked_tx,
    NULL
  };
  ComposedFixture composition;

  composition.setup = utxo_to_tx_setup;
  composition.teardown = utxo_to_tx_teardown;

  const struct CMUnitTest tests[] = {
    cmocka_unit_test_prestate_setup_teardown(
      test_utxo_to_tx_init,
      NULL,
      NULL,
      NULL
    ),
    cmocka_unit_test_prestate_setup_teardown(
      test_utxo_to_tx_add,
      fixture_setup_compose,
      fixture_teardown_compose,
      &composition
    ),
    cmocka_unit_test_prestate_setup_teardown(
      test_utxo_to_tx_add_tx,
      fixture_setup_compose,
      fixture_teardown_compose,
      &composition
    ),
    cmocka_unit_test_prestate_setup_teardown(
      test_utxo_to_tx_find,
      fixture_setup_compose,
      fixture_teardown_compose,
      &composition
    ),
    cmocka_unit_test_prestate_setup_teardown(
      test_utxo_to_tx_remove,
      fixture_setup_compose,
      fixture_teardown_compose,
      &composition
    )
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
