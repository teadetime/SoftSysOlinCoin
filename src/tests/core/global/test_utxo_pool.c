#include <stdio.h>
#include "minunit.h"
#include "utxo_pool.h"
#include "crypto.h"
#include "init_db.h"

int tests_run = 0;

Transaction *_make_tx() {
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

static char *test_utxo_pool_init() {
  utxo_pool_init_leveldb(TEST_DB_LOC);
  mu_assert(
    "UTXO DB is still null",
    utxo_pool_db != NULL
  );
  mu_assert(
    "UTXO path is null",
    utxo_pool_path != NULL
  );
  destroy_db(&utxo_pool_db, utxo_pool_path);
  return NULL;
}

static char  *test_utxo_pool_add() {
  Transaction *tx;

  tx = _make_tx();
  utxo_pool_init_leveldb(TEST_DB_LOC);
  unsigned int num_utxo_prev;
  utxo_pool_count(&num_utxo_prev);
  int ret = utxo_pool_add_leveldb(tx,0);
  unsigned int num_utxo_after;
  utxo_pool_count(&num_utxo_after);
  mu_assert(
    "Return Value indicates a failure",
    ret == 0
  );
  mu_assert(
    "UTXO pool increased in size incorrectly",
    num_utxo_after == num_utxo_prev+1
  );
  free(tx->inputs);
  free(tx->outputs);
  free(tx);
  destroy_db(&utxo_pool_db, utxo_pool_path);

  return NULL;
}

static char  *test_utxo_pool_find() {
  Transaction *tx;
  UTXO *ret_utxo = NULL;
  unsigned char hash[TX_HASH_LEN];

  tx = _make_tx();
  hash_tx(hash, tx);

  utxo_pool_init_leveldb(TEST_DB_LOC);
  utxo_pool_add_leveldb(tx, 0);
  int ret_found = utxo_pool_find_leveldb(&ret_utxo, hash, 0);
  mu_assert(
    "Found did not return proper error code",
    ret_found == 0 
  );
  mu_assert(
    "Returned utxo not different",
    ret_utxo != NULL
  );
  mu_assert(
    "Returned utxo amount if off",
    ret_utxo->amt == tx->outputs[0].amt
  );
  mu_assert(
    "Returned utxo amount if off",
    memcmp(ret_utxo->public_key_hash, tx->outputs[0].public_key_hash, PUB_KEY_HASH_LEN) == 0
  );

  free(tx->inputs);
  free(tx->outputs);
  free(tx);
  free(ret_utxo);
  destroy_db(&utxo_pool_db, utxo_pool_path);
  return NULL;
}

static char  *test_utxo_pool_remove() {
  Transaction *tx;
  UTXO  *ret_utxo = NULL;
  unsigned char hash[TX_HASH_LEN];

  tx = _make_tx();
  hash_tx(hash, tx);

  utxo_pool_init_leveldb(TEST_DB_LOC);
  utxo_pool_add_leveldb(tx, 0);
  int ret_remove = utxo_pool_remove_leveldb(hash, 0);
  
  mu_assert(
    "Remove returned failed code",
    ret_remove == 0 
  );
  int ret_found = utxo_pool_find_leveldb(&ret_utxo, hash, 0);
  mu_assert(
    "utxo was found after it was deleted?",
    ret_found != 0 
  );
  mu_assert(
    "utxo find value not null after delete",
    ret_utxo == NULL
  );

  free(tx->inputs);
  free(tx->outputs);
  free(tx);
  destroy_db(&utxo_pool_db, utxo_pool_path);
  return NULL;
}

static char *all_tests() {
  mu_run_test(test_utxo_pool_init);
  mu_run_test(test_utxo_pool_add);
  mu_run_test(test_utxo_pool_find);
  mu_run_test(test_utxo_pool_remove);
  return NULL;
}

int main() {
  create_proj_folders();
  char *result = all_tests();
  if (result != NULL) {
    printf("%s\n", result);
  } else {
    printf("utxo_pool.c passing!\n");
  }
  printf("Tests run: %d\n", tests_run);

  return result != 0;
}
