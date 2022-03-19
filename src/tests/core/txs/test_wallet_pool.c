#include <stdio.h>
#include "minunit.h"
#include "wallet_pool.h"
#include "sign_tx.h"

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
  wallet_init();
  mu_assert(
      "Wallet pool was not initialized",
      wallet_pool == NULL
  );
  mu_assert(
      "Key pool was not initialized",
      key_pool == NULL
  );
  return NULL;
}

static char  *test_wallet_pool_add() {
  Transaction *tx;
  WalletEntry *entry;
  mbedtls_ecdsa_context *key_pair;

  tx = _make_tx();
  key_pair = gen_keys();

  wallet_init();

  entry = wallet_pool_add(tx, 0, key_pair);
  mu_assert(
    "Wallet entry amt incorrect",
    entry->amt == tx->outputs[0].amt
  );
  mu_assert(
    "Wallet key pair incorrect",
    entry->key_pair == key_pair
  );
  mu_assert(
    "Wallet entry spent incorrect",
    entry->spent == 0
  );

  _free_tx(tx);
  mbedtls_ecp_keypair_free(key_pair);
  free(entry);

  return NULL;
}

static char  *test_wallet_pool_find() {
  Transaction *tx;
  WalletEntry *entry, *ret_entry;
  mbedtls_ecdsa_context *key_pair;
  unsigned char hash[TX_HASH_LEN];

  tx = _make_tx();
  hash_tx(hash, tx);
  key_pair = gen_keys();

  wallet_init();

  entry = wallet_pool_add(tx, 0, key_pair);
  ret_entry = wallet_pool_find(hash, 0);
  mu_assert(
    "Find did not return correct entry",
    ret_entry == entry
  );

  _free_tx(tx);
  mbedtls_ecp_keypair_free(key_pair);
  free(entry);

  return NULL;
}

static char  *test_wallet_pool_remove() {
  Transaction *tx;
  WalletEntry *ret_entry;
  mbedtls_ecdsa_context *key_pair;
  unsigned char hash[TX_HASH_LEN];

  tx = _make_tx();
  hash_tx(hash, tx);
  key_pair = gen_keys();

  wallet_init();

  wallet_pool_add(tx, 0, key_pair);
  wallet_pool_remove(hash, 0);
  ret_entry = wallet_pool_find(hash, 0);
  mu_assert(
    "Entry was not removed",
    ret_entry == NULL
  );

  _free_tx(tx);
  mbedtls_ecp_keypair_free(key_pair);

  return NULL;
}

static char  *test_key_pool_add() {
  mbedtls_ecdsa_context *key_pair, *ret_pair;

  key_pair = gen_keys();

  wallet_init();

  ret_pair = key_pool_add(key_pair);
  mu_assert(
    "Returned key pair is incorrect",
    key_pair == ret_pair
  );

  mbedtls_ecp_keypair_free(key_pair);

  return NULL;
}

static char  *test_key_pool_find() {
  mbedtls_ecdsa_context *key_pair, *ret_pair;
  unsigned char hash[PUB_KEY_HASH_LEN];

  key_pair = gen_keys();
  hash_pub_key(hash, key_pair);

  wallet_init();

  key_pool_add(key_pair);
  ret_pair = key_pool_find(hash);
  mu_assert(
    "Found key pair is incorrect",
    key_pair == ret_pair
  );

  mbedtls_ecp_keypair_free(key_pair);

  return NULL;
}

static char  *test_key_pool_remove() {
  mbedtls_ecdsa_context *key_pair, *ret_pair;
  unsigned char hash[PUB_KEY_HASH_LEN];

  key_pair = gen_keys();
  hash_pub_key(hash, key_pair);

  wallet_init();

  key_pool_add(key_pair);
  ret_pair = key_pool_remove(hash);
  mu_assert(
    "Returned removed key pair is incorrect",
    key_pair == ret_pair
  );
  ret_pair = key_pool_find(hash);
  mu_assert(
    "Key pair was not removed",
    ret_pair == NULL
  );

  mbedtls_ecp_keypair_free(key_pair);

  return NULL;
}

static char *all_tests() {
  mu_run_test(test_wallet_init);
  mu_run_test(test_wallet_pool_add);
  mu_run_test(test_wallet_pool_find);
  mu_run_test(test_wallet_pool_remove);
  mu_run_test(test_key_pool_add);
  mu_run_test(test_key_pool_find);
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
