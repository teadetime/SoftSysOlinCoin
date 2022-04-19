#include <stdio.h>
#include "minunit.h"
#include "wallet_pool.h"
#include "crypto.h"

int tests_run = 0;

Transaction *_make_tx() {
  Transaction *tx;
  Input *in;
  Output *out;
  mbedtls_ecdsa_context *key_pair;

  tx = malloc(sizeof(Transaction));
  in = malloc(sizeof(Input));
  out = malloc(sizeof(Output));

  memset(in, 0x25, sizeof(Input));
  key_pair = gen_keys();
  in->pub_key = &(key_pair->private_Q);

  memset(out, 0x50, sizeof(Output));

  tx->num_inputs = 1;
  tx->inputs = in;
  tx->num_outputs = 1;
  tx->outputs = out;

  return tx;
}

void _free_tx(Transaction *tx) {
  free(tx->inputs);
  free(tx->outputs);
  free(tx);
}

static char *test_wallet_init() {
  wallet_init_leveldb();
  destroy_wallet();
  int init_ret = wallet_init_leveldb();
  unsigned int wallet_count;
  wallet_pool_count(&wallet_count);
  unsigned int key_count;
  key_pool_count(&key_count);
  mu_assert(
    "Init wallet returned failure code",
    init_ret == 0
  );
  mu_assert(
    "Wallet pool is not empty",
    wallet_count == 0
  );
  mu_assert(
    "Key pool is not empty",
    key_count == 0
  );
  destroy_wallet();
  return NULL;
}

static char  *test_wallet_pool_add() {
  Transaction *tx;
  mbedtls_ecdsa_context *key_pair;

  tx = _make_tx();
  key_pair = gen_keys();

  wallet_init_leveldb();

  int ret_add = wallet_pool_build_add_leveldb(tx, 0, key_pair);
  unsigned int wallet_pool_sz;
  wallet_pool_count(&wallet_pool_sz);
  mu_assert(
    "Wallet pool add returned error status",
    ret_add == 0
  );
  mu_assert(
    "Wallet pool not correct size",
    wallet_pool_sz == 1
  );
  // Note find tests if the found data is correct
  _free_tx(tx);
  mbedtls_ecp_keypair_free(key_pair);
  destroy_wallet();
  return NULL;
}

static char  *test_wallet_pool_find() {
  Transaction *tx;
  WalletEntry *ret_entry;
  mbedtls_ecdsa_context *key_pair;
  unsigned char hash[TX_HASH_LEN];

  tx = _make_tx();
  hash_tx(hash, tx);
  key_pair = gen_keys();

  wallet_init_leveldb();

  wallet_pool_build_add_leveldb(tx, 0, key_pair);
  int ret_find = wallet_pool_find_leveldb(&ret_entry, hash, 0);
  mu_assert(
    "Wallet pool find returned an error",
    ret_find == 0
  );
  mu_assert(
    "Wallet amt incorrect",
    ret_entry->amt == tx->outputs[0].amt
  );
  mu_assert(
    "Wallet key pair private Q incorrect",
    mbedtls_ecp_point_cmp(&ret_entry->key_pair->private_Q, &key_pair->private_Q) == 0
  );
  // mu_assert(
  //   "Wallet key pair private grp incorrect",
  //   memcmp(ret_entry->key_pair, key_pair, sizeof(mbedtls_ecp_keypair)) == 0
  // );
  // mu_assert(
  //   "Wallet key pair private d incorrect",
  //   mbedtls_ecp_point_cmp(&ret_entry->key_pair->private_d, &key_pair->private_d) == 0
  // );
  mu_assert(
    "Wallet entry spent incorrect",
    ret_entry->spent == 0
  );
  _free_tx(tx);
  mbedtls_ecp_keypair_free(key_pair);
  destroy_wallet();
  return NULL;
}

static char  *test_wallet_pool_remove() {
  Transaction *tx;
  WalletEntry *ret_entry = NULL;
  mbedtls_ecdsa_context *key_pair;
  unsigned char hash[TX_HASH_LEN];

  tx = _make_tx();
  hash_tx(hash, tx);
  key_pair = gen_keys();

  wallet_init_leveldb();

  wallet_pool_build_add_leveldb(tx, 0, key_pair);
  int ret_rem = wallet_pool_remove_leveldb(hash, 0);
  int ret_find = wallet_pool_find_leveldb(&ret_entry, hash, 0);
  mu_assert(
    "Wallet remove returned error code",
    ret_rem == 0
  );
  mu_assert(
    "Entry was not removed",
    ret_entry == NULL
  );
  mu_assert(
    "Wallet found deleted entry",
    ret_find != 0
  );
  _free_tx(tx);
  mbedtls_ecp_keypair_free(key_pair);
  destroy_wallet();
  return NULL;
}

static char  *test_key_pool_add() {
  mbedtls_ecdsa_context *key_pair;

  key_pair = gen_keys();

  wallet_init_leveldb();

  int ret_add = key_pool_add_leveldb(key_pair);
  mu_assert(
    "Keypool add returned failure",
    ret_add == 0
  );

  mbedtls_ecp_keypair_free(key_pair);
  destroy_wallet();
  return NULL;
}

static char  *test_key_pool_find() {
  mbedtls_ecdsa_context *key_pair, *ret_pair;
  unsigned char hash[PUB_KEY_HASH_LEN];

  key_pair = gen_keys();
  hash_pub_key(hash, key_pair);

  wallet_init_leveldb();

  key_pool_add_leveldb(key_pair);
  int ret_find = key_pool_find_leveldb(&ret_pair, hash);
  mu_assert(
    "Keypool find returned failure",
    ret_find == 0
  );

  mu_assert(
    "Key Pool pair private Q incorrect",
    mbedtls_ecp_point_cmp(&ret_pair->private_Q, &key_pair->private_Q) == 0
  );
  mbedtls_ecp_keypair_free(key_pair);
  destroy_wallet();
  return NULL;
}

static char  *test_output_unlockable() {
  Transaction *tx;
  mbedtls_ecdsa_context *key_pair, *ret_pair;
  tx = _make_tx();

  key_pair = gen_keys();
  hash_pub_key(tx->outputs[0].public_key_hash, key_pair);

  wallet_init_leveldb();

  key_pool_add_leveldb(key_pair);
  ret_pair = check_if_output_unlockable_leveldb(tx, 0);
  mu_assert(
    "Returned pair is NULL",
    ret_pair != NULL
  );
  mu_assert(
    "Returned key pair private Q incorrect",
    mbedtls_ecp_point_cmp(&ret_pair->private_Q, &key_pair->private_Q) == 0
  );
  mbedtls_ecp_keypair_free(key_pair);
  destroy_wallet();
  return NULL;
}

static char  *test_key_pool_remove() {
  mbedtls_ecdsa_context *key_pair, *ret_pair = NULL;
  unsigned char hash[PUB_KEY_HASH_LEN];

  key_pair = gen_keys();
  hash_pub_key(hash, key_pair);

  wallet_init_leveldb();

  key_pool_add_leveldb(key_pair);
  int ret_rem = key_pool_remove_leveldb(hash);
  mu_assert(
    "Remove returned error code",
    ret_rem == 0
  );
  int ret_found = key_pool_find_leveldb(&ret_pair, hash);
  mu_assert(
    "Key pair was found after removed",
    ret_found != 0
  );
  mu_assert(
    "Key pair return value not null",
    ret_pair == NULL
  );

  mbedtls_ecp_keypair_free(key_pair);
  destroy_wallet();
  return NULL;
}

static char *all_tests() {
  mu_run_test(test_wallet_init);
  mu_run_test(test_wallet_pool_add);
  mu_run_test(test_wallet_pool_find);
  mu_run_test(test_wallet_pool_remove);
  mu_run_test(test_key_pool_add);
  mu_run_test(test_key_pool_find);
  mu_run_test(test_output_unlockable);
  mu_run_test(test_key_pool_remove);
  return NULL;
}

int main() {
  char *result = all_tests();
  if (result != NULL) {
    printf("%s\n", result);
  } else {
    printf("wallet_pool.c passing!\n");
  }
  printf("Tests run: %d\n", tests_run);

  return result != 0;
}
