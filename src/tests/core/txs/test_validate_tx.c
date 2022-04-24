#include <stdlib.h>
#include <stdio.h>
#include "utxo_to_tx.h"
#include "crypto.h"
#include "utxo_pool.h"
#include "mempool.h"
#include "minunit.h"
#include "validate_tx.h"
#include "init_db.h"

int tests_run = 0;

mbedtls_ecdsa_context *last_key_pair;
Transaction *_make_tx() {
  Transaction *tx;
  Input *in;
  Output *out;
  mbedtls_ecdsa_context *key_pair;
  mbedtls_ecdsa_context *output_key_pair;
  tx = malloc(sizeof(Transaction));
  in = malloc(sizeof(Input));
  out = malloc(sizeof(Output));

  memset(in, 0, sizeof(Input));
  key_pair = gen_keys();
  in->pub_key = &(key_pair->private_Q);
  in->sig_len = 0; 

  memset(out, 0, sizeof(Output));

  output_key_pair = gen_keys();
  hash_pub_key(out->public_key_hash, output_key_pair);
  last_key_pair = output_key_pair;
  tx->num_inputs = 1;
  tx->inputs = in;
  tx->num_outputs = 1;
  tx->outputs = out;

  return tx;
}

Transaction *build_complex_tx(){
  Transaction *input_tx, *tx1;
  input_tx = _make_tx();
  input_tx->outputs[0].amt = 100;
  utxo_pool_init_leveldb(TEST_DB_LOC);
  utxo_pool_add_leveldb(input_tx, 0);
  utxo_to_tx_add_tx(input_tx);
  mbedtls_ecdsa_context *input_tx_context = last_key_pair;
  
  tx1 = _make_tx();
  hash_tx(tx1->inputs[0].prev_tx_id, input_tx);
  tx1->inputs[0].prev_utxo_output = 0;
  tx1->inputs[0].pub_key = &input_tx_context->private_Q;
  tx1->outputs[0].amt = 90; 

  unsigned char temp_hash[TX_HASH_LEN];
  
  hash_tx(temp_hash, tx1); // This hash should balnk signature
  tx1->inputs[0].sig_len = write_sig(tx1->inputs[0].signature, SIGNATURE_LEN, temp_hash, TX_HASH_LEN, input_tx_context);
  return tx1;
}

static char  *test_create_blank_sig_tx_hash() {
  Transaction *test_blank = _make_tx();
  test_blank->inputs[0].sig_len = 10; // Set bad data so that the hash is different
  unsigned char test_hash[TX_HASH_LEN];
  unsigned char test_hash_blank[TX_HASH_LEN];
  
  hash_tx(test_hash, test_blank);
  create_blank_sig_txhash(test_hash_blank, test_blank);
  // Accept tests because accept only runs if validation pasts
  mu_assert(
    "Hash of a blank tx is same as original",
    memcmp(test_hash, test_hash_blank, TX_HASH_LEN) != 0
  );

  // Further testing will verify if it can check a signature
  return NULL;
}

static char  *test_check_input_unlockable() {
  unsigned char tx1_blank[TX_HASH_LEN];
  Transaction *tx1 = build_complex_tx();
  create_blank_sig_txhash(tx1_blank, tx1);

  for(unsigned int i = 0; i < tx1->num_inputs; i++){
    mu_assert(
      "Input Not unlockable",
      check_input_unlockable(&tx1->inputs[i], tx1_blank) == 0
    );
  }
  // Check bad
  unsigned char bad_hash[TX_HASH_LEN] = {'0'};
  mu_assert(
    "Blank hash unlocked input it should not",
    check_input_unlockable(&tx1->inputs[0], bad_hash) != 0
  );
  tx1->inputs[0].sig_len -= 1;
  mu_assert(
    "Signature modification did not break validation",
    check_input_unlockable(&tx1->inputs[0], bad_hash) != 0
  );
  destroy_db(&utxo_pool_db, utxo_pool_path);
  return NULL;
}

static char *test_validate_input_matches_utxo_pool() {
  //Clean out the utxo->tx mapping after calling _build_tx
  UTXOToTx *current_utxo_to_tx, *tmp;
  HASH_ITER(hh, utxo_to_tx, current_utxo_to_tx, tmp) {
    HASH_DEL(utxo_to_tx, current_utxo_to_tx);  /* delete; users advances to next */
    free(current_utxo_to_tx);             /* Free the mallocced data */
  }

  Transaction *tx1 = build_complex_tx();
  for(unsigned int i = 0; i < tx1->num_inputs; i++){
    mu_assert(
      "Inputs are not found in the utxo pool",
      validate_input_matches_utxopool(&tx1->inputs[i]) == 0
    );
  }
  destroy_db(&utxo_pool_db, utxo_pool_path);
  return NULL;
}

static char *test_validate_input() {
  //Clean out the utxo->tx mapping after calling _build_tx
  UTXOToTx *current_utxo_to_tx, *tmp;
  unsigned char tx1_blank[TX_HASH_LEN];
  HASH_ITER(hh, utxo_to_tx, current_utxo_to_tx, tmp) {
    HASH_DEL(utxo_to_tx, current_utxo_to_tx);  /* delete; users advances to next */
    free(current_utxo_to_tx);             /* Free the mallocced data */
  }
  Transaction *tx1 = build_complex_tx();

  create_blank_sig_txhash(tx1_blank, tx1);
  for(unsigned int i = 0; i < tx1->num_inputs; i++){
    mu_assert(
      "Validating input failed",
      validate_input(&tx1->inputs[i], tx1_blank) == 0
    );
  }
  destroy_db(&utxo_pool_db, utxo_pool_path);
  return NULL;
}

static char *test_validate_tx_not_null() {
  Transaction *tx = _make_tx();
  mu_assert(
    "Good Transaction has NULL Parts",
    validate_tx_parts_not_null(tx) == 0
  );
  tx->inputs = NULL;
  mu_assert(
    "Transaction with NULL inputs not detected",
    validate_tx_parts_not_null(tx) != 0
  );

  tx = _make_tx();
  tx->num_outputs = 0;
  mu_assert(
    "Transaction with 0 outputs not detected",
    validate_tx_parts_not_null(tx) != 0
  );

  tx = _make_tx();
  tx->outputs = NULL;
  mu_assert(
    "Transaction with NULL outputs not detected",
    validate_tx_parts_not_null(tx) != 0
  );
  return NULL;
}

static char *test_validate_coinbase_tx_parts_not_null(){
  Transaction *tx = _make_tx();
  mu_assert(
    "Transaction with NULL outputs not detected",
    validate_coinbase_tx_parts_not_null(tx) != 0
  );

  tx->num_inputs = 0;
  mu_assert(
    "Transaction with NULL outputs not detected",
    validate_coinbase_tx_parts_not_null(tx) != 0
  );

  tx->inputs = NULL;
  mu_assert(
    "Coinbase TX with NULL inputs",
    validate_coinbase_tx_parts_not_null(tx) == 0
  );
  return NULL;
}

static char *test_validate_shared_tx(){
  //Clean out the utxo->tx mapping after calling _build_tx
  UTXOToTx *current_utxo_to_tx, *tmp;
  HASH_ITER(hh, utxo_to_tx, current_utxo_to_tx, tmp) {
    HASH_DEL(utxo_to_tx, current_utxo_to_tx);  /* delete; users advances to next */
    free(current_utxo_to_tx);             /* Free the mallocced data */
  }
  Transaction *tx1 = build_complex_tx();
  mu_assert(
    "Shared Transaction Validation Failed",
    validate_tx_shared(tx1) == 0
  );
  destroy_db(&utxo_pool_db, utxo_pool_path);
  return NULL;
}

static char *test_check_tx_double_spent(){
  //Clean out the utxo->tx mapping after calling _build_tx
  UTXOToTx *current_utxo_to_tx, *tmp;
  HASH_ITER(hh, utxo_to_tx, current_utxo_to_tx, tmp) {
    HASH_DEL(utxo_to_tx, current_utxo_to_tx);  /* delete; users advances to next */
    free(current_utxo_to_tx);             /* Free the mallocced data */
  }
  Transaction *tx1 = build_complex_tx();
  mu_assert(
    "Double spend detected when there isn't one",
    check_tx_double_spent(tx1) == 0
  );
  destroy_db(&utxo_pool_db, utxo_pool_path);
  return NULL;
}

static char *test_inputs_not_in_mempool(){
  //Clean out the utxo->tx mapping after calling _build_tx
  UTXOToTx *current_utxo_to_tx, *tmp;
  HASH_ITER(hh, utxo_to_tx, current_utxo_to_tx, tmp) {
    HASH_DEL(utxo_to_tx, current_utxo_to_tx);  /* delete; users advances to next */
    free(current_utxo_to_tx);             /* Free the mallocced data */
  }
  Transaction *tx1 = build_complex_tx();
  mu_assert(
    "Incorrectly finds inputs in utxo_to_tx mapping when not there",
    check_inputs_not_in_mempool(tx1) == 0
  );

  utxo_to_tx_add_tx(tx1);
  printf("UTXO map size: %i", HASH_COUNT(utxo_to_tx));
  mu_assert(
    "Tx not rejected when inputs in utxo_to_tx mapping",
    check_inputs_not_in_mempool(tx1) != 0
  );
  destroy_db(&utxo_pool_db, utxo_pool_path);
  return NULL;
}

static char *test_validate_incoming_tx(){
  //Clean out the utxo->tx mapping after calling _build_tx
  UTXOToTx *current_utxo_to_tx, *tmp;
  HASH_ITER(hh, utxo_to_tx, current_utxo_to_tx, tmp) {
    HASH_DEL(utxo_to_tx, current_utxo_to_tx);  /* delete; users advances to next */
    free(current_utxo_to_tx);             /* Free the mallocced data */
  }
  Transaction *tx1 = build_complex_tx();
  mu_assert(
    "Valid Transaction rejected by incoming validation",
    validate_tx_incoming(tx1) == 0
  );
  destroy_db(&utxo_pool_db, utxo_pool_path);
  return NULL;
}




static char *all_tests(){
  utxo_to_tx_init();
  mu_run_test(test_create_blank_sig_tx_hash);
  mu_run_test(test_check_input_unlockable);
  mu_run_test(test_validate_input_matches_utxo_pool);
  mu_run_test(test_validate_input);
  mu_run_test(test_validate_tx_not_null);
  mu_run_test(test_validate_coinbase_tx_parts_not_null);
  mu_run_test(test_validate_shared_tx);
  mu_run_test(test_check_tx_double_spent);
  mu_run_test(test_inputs_not_in_mempool);
  mu_run_test(test_validate_incoming_tx);

  return NULL;
}

int main() {
  create_proj_folders();
  char *result = all_tests();
  if (result != NULL) {
    printf("%s\n", result);
  } else {
    printf("validate_tx.c passing!\n");
  }
  printf("Tests run: %d\n", tests_run);

  return result != 0;
}
