#include <stdio.h>
#include "utxo_to_tx.h"
#include "mempool.h"
#include "minunit.h"
#include "handle_tx.h"
#include "crypto.h"
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
  utxo_pool_init_leveldb();
  utxo_pool_add_leveldb(input_tx, 0);

  mbedtls_ecdsa_context *input_tx_context = malloc(sizeof(mbedtls_ecdsa_context));
  memcpy(input_tx_context, last_key_pair, sizeof(mbedtls_ecdsa_context));
  tx1 = _make_tx();
  hash_tx(tx1->inputs[0].prev_tx_id, input_tx);
  tx1->inputs[0].prev_utxo_output = 0;
  tx1->inputs[0].pub_key = &input_tx_context->private_Q;
  tx1->outputs[0].amt = 90; 

  unsigned char temp_hash[TX_HASH_LEN];
  hash_tx(temp_hash, tx1);
  tx1->inputs[0].sig_len = write_sig(tx1->inputs[0].signature, SIGNATURE_LEN, temp_hash, TX_HASH_LEN, input_tx_context);
  return tx1;
}

static char *test_handle_tx(){
  //Clean out the utxo->tx mapping after calling _build_tx
  Transaction *tx = build_complex_tx();
  unsigned char temp_hash[TX_HASH_LEN];
  mu_assert(
    "UTXO_to_tx mapping is not starting empty",
    HASH_COUNT(utxo_to_tx) == 0
  );
  mu_assert(
    "mempool is not starting empty",
    HASH_COUNT(mempool) == 0
  );

  int ret = handle_tx(tx);
  mu_assert(
    "Hanlde TX Failed when success expected",
    ret == 0
  );
  mu_assert(
    "UTXO_to_tx has different number of entries than expected",
    HASH_COUNT(utxo_to_tx) == 1
  );
  mu_assert(
    "mempool is is not expected size after handling transaction",
    HASH_COUNT(mempool) == 1
  );
  mu_assert(
    "UTXO_to_tx mapping doens't contain expected data",
    utxo_to_tx_find(temp_hash, tx->inputs[0].prev_tx_id, 0) == 0
  );
  hash_tx(temp_hash, tx);
  mu_assert(
    "Mempool entry matching tx not found",
    mempool_find(temp_hash) != NULL
  );
  destroy_db(&utxo_pool_db, utxo_pool_path);
  return NULL;
}




static char *all_tests(){
  mempool_init();
  utxo_to_tx_init();
  mu_run_test(test_handle_tx);
  return NULL;
}

int main() {
  char *result = all_tests();
  if (result != NULL) {
    printf("%s\n", result);
  } else {
    printf("validate_tx.c passing!\n");
  }
  printf("Tests run: %d\n", tests_run);

  return result != 0;
}
