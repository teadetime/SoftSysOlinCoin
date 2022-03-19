#include <stdio.h>
#include "minunit.h"
#include "mempool.h"
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

static char *test_mempool_init() {
  mempool_init();
  mu_assert(
      "Mempool was not initialized",
      mempool == NULL
  );
  return NULL;
}

static char  *test_mempool_add() {
  Transaction *tx, *ret_tx;

  tx = _make_tx();
  mempool_init();
  ret_tx = mempool_add(tx);
  mu_assert(
    "Add did not return correct tx",
    ret_tx == tx
  );

  free(tx->inputs);
  free(tx->outputs);
  free(tx);

  return NULL;
}

static char  *test_mempool_find() {
  Transaction *tx, *ret_tx;
  unsigned char hash[TX_HASH_LEN];

  tx = _make_tx();
  hash_tx(hash, tx);

  mempool_init();
  mempool_add(tx);
  ret_tx = mempool_find(hash);
  mu_assert(
    "Find did not return correct tx",
    ret_tx == tx
  );

  free(tx->inputs);
  free(tx->outputs);
  free(tx);

  return NULL;
}

static char  *test_mempool_remove() {
  Transaction *tx, *ret_tx;
  unsigned char hash[TX_HASH_LEN];

  tx = _make_tx();
  hash_tx(hash, tx);

  mempool_init();
  mempool_add(tx);
  ret_tx = mempool_remove(hash);
  mu_assert(
    "Remove did not return correct tx",
    ret_tx == tx
  );

  ret_tx = mempool_find(hash);
  mu_assert(
    "Tx was not removed",
    ret_tx == NULL
  );

  free(tx->inputs);
  free(tx->outputs);
  free(tx);

  return NULL;
}

static char *all_tests() {
  mu_run_test(test_mempool_init);
  mu_run_test(test_mempool_add);
  mu_run_test(test_mempool_find);
  mu_run_test(test_mempool_remove);
  return NULL;
}

int main() {
  char *result = all_tests();
  if (result != NULL) {
    printf("%s\n", result);
  } else {
    printf("mempool.c passing!\n");
  }
  printf("Tests run: %d\n", tests_run);

  return result != 0;
}
