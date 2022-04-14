#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "minunit.h"
#include "base_tx.h"
#include "ser_tx.h"
#include "crypto.h"

int tests_run = 0;

Transaction *_make_tx() {
  Transaction *a_Tx = malloc(sizeof(Transaction));
  a_Tx->num_inputs = 2;
  a_Tx->num_outputs = 1;

  a_Tx->inputs = malloc(sizeof(Input) * (a_Tx->num_inputs));
  a_Tx->outputs = malloc(sizeof(Output) * (a_Tx->num_outputs));

  Output *an_Output = &(a_Tx->outputs[0]);
  memset(an_Output, 0, sizeof(Output));
  an_Output->amt = 5;
  strcpy((char*)an_Output->public_key_hash, "a_val");

  Input *input_1 = &(a_Tx->inputs[0]);
  memset(input_1, 0, sizeof(Input));
  input_1->prev_utxo_output = 2;
  mbedtls_ecdsa_context *key_pair_1 = gen_keys();
  input_1->pub_key = &key_pair_1->private_Q;

  Input *input_2 = &(a_Tx->inputs[1]);
  memset(input_2, 0, sizeof(Input));
  input_2->prev_utxo_output = 4;
  mbedtls_ecdsa_context *key_pair_2 = gen_keys();
  input_2->pub_key = &key_pair_2->private_Q;

  return a_Tx;
}

static char  *test_copy_tx() {
  Transaction *tx, *copy;
  unsigned char tx_hash[TX_HASH_LEN], copy_hash[TX_HASH_LEN];

  tx = _make_tx();
  hash_tx(tx_hash, tx);

  copy = copy_tx(tx);
  hash_tx(copy_hash, copy);

  mu_assert(
    "Copy produces different hash",
    memcmp(tx_hash, copy_hash, TX_HASH_LEN) == 0
  );
  return NULL;
}

static char *all_tests() {
  mu_run_test(test_copy_tx);
  return NULL;
}

int main() {
    char *result = all_tests();
  if (result != NULL) {
    printf("%s\n", result);
  } else {
    printf("base_tx.c passing!\n");
  }
  printf("Tests run: %d\n", tests_run);

  return result != 0;
}
