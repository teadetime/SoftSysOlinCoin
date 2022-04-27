#include <stdio.h>

#include <setjmp.h>
#include <cmocka.h>

#include "wallet_pool.h"
#include "crypto.h"
#include "fixtures_wallet.h"
#include "fixtures_global.h"

static void test_wallet_init(void **state) {
  int init_ret = wallet_init_leveldb(TEST_DB_LOC);
  unsigned int wallet_count, key_count;
  (void)state;
  wallet_pool_count(&wallet_count);
  key_pool_count(&key_count);
  assert_int_equal(init_ret, 0);
  assert_int_equal(wallet_count, 0);
  assert_int_equal(key_count, 0);
  destroy_wallet();
}

static void test_wallet_pool_add(void **state) {
  Transaction *tx;
  mbedtls_ecdsa_context *key_pair;

  tx = ((void**)*state)[0];
  key_pair = ((void**)*state)[1];

  int ret_add = wallet_pool_build_add_leveldb(tx, 0, key_pair);
  unsigned int wallet_pool_sz;
  wallet_pool_count(&wallet_pool_sz);

  assert_int_equal(ret_add, 0);
  assert_int_equal(wallet_pool_sz, 1);
}

static void test_wallet_pool_find(void **state) {
  Transaction *tx;
  WalletEntry *ret_entry;
  mbedtls_ecdsa_context *key_pair;
  unsigned char hash[TX_HASH_LEN];

  tx = ((void**)*state)[0];
  key_pair = ((void**)*state)[1];
  hash_tx(hash, tx);

  wallet_pool_build_add_leveldb(tx, 0, key_pair);
  int ret_find = wallet_pool_find_leveldb(&ret_entry, hash, 0);

  assert_int_equal(ret_find, 0);
  assert_int_equal(ret_entry->amt, tx->outputs[0].amt);
  assert_int_equal(ret_entry->spent, 0);
  assert_true(
    mbedtls_ecp_point_cmp(
      &ret_entry->key_pair->private_Q,
      &key_pair->private_Q
    ) == 0
  );
  assert_true(
    mbedtls_mpi_cmp_mpi(
      &ret_entry->key_pair->private_d,
      &key_pair->private_d
    ) == 0
  );
}

static void test_wallet_pool_remove(void **state) {
  Transaction *tx;
  WalletEntry *ret_entry = NULL;
  mbedtls_ecdsa_context *key_pair;
  unsigned char hash[TX_HASH_LEN];

  tx = ((void**)*state)[0];
  key_pair = ((void**)*state)[1];
  hash_tx(hash, tx);

  wallet_pool_build_add_leveldb(tx, 0, key_pair);
  int ret_rem = wallet_pool_remove_leveldb(hash, 0);
  int ret_find = wallet_pool_find_leveldb(&ret_entry, hash, 0);

  assert_int_equal(ret_rem, 0);
  assert_ptr_equal(ret_entry, NULL);
  assert_int_not_equal(ret_find, 0);
}

static void test_key_pool_add(void **state) {
  mbedtls_ecdsa_context *key_pair;

  key_pair = *state;

  int ret_add = key_pool_add_leveldb(key_pair);

  assert_int_equal(ret_add, 0);
}

static void test_key_pool_find(void **state) {
  mbedtls_ecdsa_context *key_pair, *ret_pair;
  unsigned char hash[PUB_KEY_HASH_LEN];

  key_pair = *state;
  hash_pub_key(hash, key_pair);

  key_pool_add_leveldb(key_pair);
  int ret_find = key_pool_find_leveldb(&ret_pair, hash);

  assert_int_equal(ret_find, 0);
  assert_true(
    mbedtls_ecp_point_cmp(
      &ret_pair->private_Q,
      &key_pair->private_Q
    ) == 0
  );
}

static void test_output_unlockable(void **state) {
  Transaction *tx;
  mbedtls_ecdsa_context *key_pair, *ret_pair;

  tx = ((void**)*state)[0];
  key_pair = ((void**)*state)[1];
  hash_pub_key(tx->outputs[0].public_key_hash, key_pair);

  key_pool_add_leveldb(key_pair);
  ret_pair = check_if_output_unlockable_leveldb(tx, 0);
  assert_ptr_not_equal(ret_pair, NULL);
  assert_true(
    mbedtls_ecp_point_cmp(
      &ret_pair->private_Q,
      &key_pair->private_Q
    ) == 0
  );
}

static void test_key_pool_remove(void **state) {
  mbedtls_ecdsa_context *key_pair, *ret_pair = NULL;
  unsigned char hash[PUB_KEY_HASH_LEN];

  key_pair = *state;
  hash_pub_key(hash, key_pair);

  key_pool_add_leveldb(key_pair);

  int ret_rem = key_pool_remove_leveldb(hash);
  assert_int_equal(ret_rem, 0);

  int ret_found = key_pool_find_leveldb(&ret_pair, hash);
  assert_int_not_equal(ret_found, 0);
  assert_ptr_equal(ret_pair, NULL);
}

int main() {
  int (*wallet_pool_setup[])(void**) = {
    fixture_setup_wallet,
    fixture_setup_unlinked_tx_keypair,
    NULL
  };
  int (*wallet_pool_teardown[])(void**) = {
    fixture_teardown_wallet,
    fixture_teardown_unlinked_tx_keypair,
    NULL
  };
  ComposedFixture wallet_pool_composition;
  wallet_pool_composition.setup = wallet_pool_setup;
  wallet_pool_composition.teardown = wallet_pool_teardown;

  int (*key_pool_setup[])(void**) = {
    fixture_setup_wallet,
    fixture_setup_unlinked_keypair,
    NULL
  };
  int (*key_pool_teardown[])(void**) = {
    fixture_teardown_wallet,
    fixture_teardown_unlinked_keypair,
    NULL
  };
  ComposedFixture key_pool_composition;
  key_pool_composition.setup = key_pool_setup;
  key_pool_composition.teardown = key_pool_teardown;

  const struct CMUnitTest tests[] = {
    cmocka_unit_test_prestate_setup_teardown(
      test_wallet_init,
      NULL,
      NULL,
      NULL
    ),
    cmocka_unit_test_prestate_setup_teardown(
      test_wallet_pool_add,
      fixture_setup_compose,
      fixture_teardown_compose,
      &wallet_pool_composition
    ),
    cmocka_unit_test_prestate_setup_teardown(
      test_wallet_pool_find,
      fixture_setup_compose,
      fixture_teardown_compose,
      &wallet_pool_composition
    ),
    cmocka_unit_test_prestate_setup_teardown(
      test_wallet_pool_remove,
      fixture_setup_compose,
      fixture_teardown_compose,
      &wallet_pool_composition
    ),
    cmocka_unit_test_prestate_setup_teardown(
      test_key_pool_add,
      fixture_setup_compose,
      fixture_teardown_compose,
      &key_pool_composition
    ),
    cmocka_unit_test_prestate_setup_teardown(
      test_key_pool_find,
      fixture_setup_compose,
      fixture_teardown_compose,
      &key_pool_composition
    ),
    cmocka_unit_test_prestate_setup_teardown(
      test_output_unlockable,
      fixture_setup_compose,
      fixture_teardown_compose,
      &wallet_pool_composition
    ),
    cmocka_unit_test_prestate_setup_teardown(
      test_key_pool_remove,
      fixture_setup_compose,
      fixture_teardown_compose,
      &key_pool_composition
    ),
  };

  return cmocka_run_group_tests(tests, fixture_clear_wallet, NULL);
}
