#include <stdio.h>
#include "minunit.h"
#include "utxo_to_tx.h"
#include "sign_tx.h"

int tests_run = 0;

Transaction *_make_tx() {
  Transaction *tx;
  Input *in;
  Output *out;
  mbedtls_ecdsa_context *key_pair;

  tx = malloc(sizeof(Transaction));
  in = malloc(sizeof(Input) * 2);
  out = malloc(sizeof(Output));

  memset(&in[0], 0x2, sizeof(Input));
  key_pair = gen_keys();
  in[0].pub_key = &(key_pair->private_Q);

  memset(&in[1], 0x8, sizeof(Input));
  key_pair = gen_keys();
  in[1].pub_key = &(key_pair->private_Q);

  memset(out, 0, sizeof(Output));

  tx->num_inputs = 2;
  tx->inputs = in;
  tx->num_outputs = 1;
  tx->outputs = out;

  return tx;
}

void _free_tx(Transaction *tx) {
  if (tx->inputs != NULL)
    free(tx->inputs);
  if (tx->outputs != NULL)
    free(tx->outputs);
  free(tx);
}

static char *test_utxo_to_tx_init() {
  utxo_to_tx_init();
  mu_assert(
      "UTXO to Tx mapping was not initialized",
      utxo_to_tx == NULL
  );
  return NULL;
}

static char  *test_utxo_to_tx_add() {
  Transaction *tx;
  int ret_val;
  Input *in;
  unsigned char tx_hash[TX_HASH_LEN];

  tx = _make_tx();
  utxo_to_tx_init();

  in = &tx->inputs[0];
  hash_tx(tx_hash, tx);
  ret_val = utxo_to_tx_add(in->prev_tx_id, in->prev_utxo_output, tx_hash);

  mu_assert(
    "Return value indicates failure",
    ret_val == 0
  );

  // Relies on UTHash storing new additions as first value in utxo_to_tx global
  // variable, which *should* always happen since it starts as NULL
  mu_assert(
    "Stored UTXO transaction hash incorrect",
    memcmp(
      utxo_to_tx->id.tx_hash,
      in->prev_tx_id,
      TX_HASH_LEN
    ) == 0
  );
  mu_assert(
    "Stored UTXO vout incorrect",
      utxo_to_tx->id.vout == in->prev_utxo_output
  );
  mu_assert(
    "Stored transaction hash incorrect",
    memcmp(
      utxo_to_tx->tx_hash,
      tx_hash,
      TX_HASH_LEN
    ) == 0
  );

  _free_tx(tx);

  return NULL;
}

static char  *test_utxo_to_tx_add_tx() {
  Transaction *tx;
  int ret_val, count;
  unsigned char tx_hash[TX_HASH_LEN];

  tx = _make_tx();
  utxo_to_tx_init();

  hash_tx(tx_hash, tx);
  ret_val = utxo_to_tx_add_tx(tx);

  mu_assert(
    "Return value indicates failure",
    ret_val == 0
  );

  // add_tx() calls add() under the hood, so we just test to ensure something
  // was added, and not what was added. This is better served by a mock
  count = HASH_COUNT(utxo_to_tx);
  mu_assert(
    "Incorrect final hashmap length",
    count == 2
  );

  _free_tx(tx);

  return NULL;
}

static char  *test_utxo_to_tx_find() {
  Transaction *tx;
  Input *in;
  int ret_val;
  unsigned char tx_hash[TX_HASH_LEN], ret_hash[TX_HASH_LEN];

  tx = _make_tx();
  in = &tx->inputs[0];
  hash_tx(tx_hash, tx);

  utxo_to_tx_init();
  utxo_to_tx_add_tx(tx);
  ret_val = utxo_to_tx_find(ret_hash, in->prev_tx_id, in->prev_utxo_output);

  mu_assert(
    "Return value indicates failure",
    ret_val == 0
  );
  mu_assert(
    "Find did not return correct transacation hash",
    memcmp(tx_hash, ret_hash, TX_HASH_LEN) == 0
  );

  _free_tx(tx);

  return NULL;
}

static char  *test_utxo_to_tx_remove() {
  Transaction *tx;
  Input *in;
  int ret_val;

  tx = _make_tx();
  in = &tx->inputs[0];

  utxo_to_tx_init();
  utxo_to_tx_add_tx(tx);

  ret_val = utxo_to_tx_remove(in->prev_tx_id, in->prev_utxo_output);
  mu_assert(
    "Return value indicates failure",
    ret_val == 0
  );

  ret_val = utxo_to_tx_find(NULL, in->prev_tx_id, in->prev_utxo_output);
  mu_assert(
    "Entry was not removed",
    ret_val == 1
  );

  _free_tx(tx);

  return NULL;
}

static char *all_tests() {
  mu_run_test(test_utxo_to_tx_init);
  mu_run_test(test_utxo_to_tx_add);
  mu_run_test(test_utxo_to_tx_add_tx);
  mu_run_test(test_utxo_to_tx_find);
  mu_run_test(test_utxo_to_tx_remove);
  return NULL;
}

int main() {
  char *result = all_tests();
  if (result != NULL) {
    printf("%s\n", result);
  } else {
    printf("utxo_to_tx.c passing!\n");
  }
  printf("Tests run: %d\n", tests_run);

  return result != 0;
}
