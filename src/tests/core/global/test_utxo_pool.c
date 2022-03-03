#include <stdio.h>
#include "minunit.h"
#include "utxo_pool.h"

int tests_run = 0;

Transaction *_make_tx() {
  Transaction *tx;
  Input *in;
  Output *out;

  tx = malloc(sizeof(Transaction));
  in = malloc(sizeof(Input));
  out = malloc(sizeof(Output));

  memset(in, 0, sizeof(Input));
  memset(out, 0, sizeof(Output));

  tx->num_inputs = 1;
  tx->inputs = in;
  tx->num_outputs = 1;
  tx->outputs = out;

  return tx;
}

static char *test_utxo_pool_init() {
  utxo_pool_init();
  mu_assert(
      "UTXO pool was not initialized",
      utxo_pool == NULL
  );
  return NULL;
}

static char  *test_utxo_pool_add() {
  Transaction *tx;
  UTXO *ret_utxo;

  tx = _make_tx();
  utxo_pool_init();
  ret_utxo = utxo_pool_add(tx, 0);

  mu_assert(
    "UTXO amt incorrect",
    ret_utxo->amt == tx->outputs[0].amt
  );
  mu_assert(
    "UTXO hash incorrect",
    memcmp(
      ret_utxo->public_key_hash,
      tx->outputs[0].public_key_hash,
      LOCK_SCRIPT_LEN
    ) == 0
  );
  mu_assert(
    "UTXO spent incorrect",
    ret_utxo->spent == 0
  );

  free(tx->inputs);
  free(tx->outputs);
  free(tx);
  free(ret_utxo);

  return NULL;
}

static char  *test_utxo_pool_find() {
  Transaction *tx;
  UTXO *new_utxo, *ret_utxo;
  unsigned char hash[TX_HASH_LEN];

  tx = _make_tx();
  hash_tx(hash, tx);

  utxo_pool_init();
  new_utxo = utxo_pool_add(tx, 0);
  ret_utxo = utxo_pool_find(hash, 0);
  mu_assert(
    "Find did not return correct utxo",
    ret_utxo == new_utxo
  );

  free(tx->inputs);
  free(tx->outputs);
  free(tx);
  free(ret_utxo);

  return NULL;
}

static char  *test_utxo_pool_remove() {
  Transaction *tx;
  UTXO *new_utxo, *ret_utxo;
  unsigned char hash[TX_HASH_LEN];

  tx = _make_tx();
  hash_tx(hash, tx);

  utxo_pool_init();
  new_utxo = utxo_pool_add(tx, 0);
  ret_utxo = utxo_pool_remove(hash, 0);
  mu_assert(
    "Remove did not return correct tx",
    ret_utxo == new_utxo
  );

  ret_utxo = utxo_pool_find(hash, 0);
  mu_assert(
    "Tx was not removed",
    ret_utxo == NULL
  );

  free(tx->inputs);
  free(tx->outputs);
  free(tx);
  free(new_utxo);

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
  char *result = all_tests();
  if (result != NULL) {
    printf("%s\n", result);
  } else {
    printf("utxo_pool.c passing!\n");
  }
  printf("Tests run: %d\n", tests_run);

  return result != 0;
}
