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

UTXO *_make_utxo() {
  UTXO *utxo;
  utxo = malloc(sizeof(UTXO));
  utxo->amt = 100;
  memset(utxo->public_key_hash, 0xAA, PUB_KEY_HASH_LEN);
  return utxo;
}

static char  *test_ser_tx() {
  Transaction *a_Tx = _make_tx();
  unsigned char *char_tx = ser_tx_alloc(a_Tx);
  Transaction *other_tx = deser_tx_alloc(char_tx);

  mu_assert(
    "Num inputs doesn't match after serialization",
    a_Tx->num_inputs == other_tx->num_inputs
  );
  mu_assert(
    "Num outputs doesn't match after serialization",
    a_Tx->num_outputs == other_tx->num_outputs
  );

  // Test Inputs
  unsigned char buf_1[PUB_KEY_SER_LEN];
  unsigned char buf_2[PUB_KEY_SER_LEN];
  for (size_t i = 0; i < a_Tx->num_inputs; i++) {
    mu_assert(
      "Input vout doesn't match up after de/serialization",
      a_Tx->inputs[i].prev_utxo_output == other_tx->inputs[i].prev_utxo_output
    );
    mu_assert(
      "Input tx hash doesn't match up after de/serialization",
      memcmp(
        a_Tx->inputs[i].prev_tx_id,
        other_tx->inputs[i].prev_tx_id,
        TX_HASH_LEN
      ) == 0
    );
    mu_assert(
      "Input signature length doesn't match up after de/serialization",
      a_Tx->inputs[i].sig_len == other_tx->inputs[i].sig_len
    );
    mu_assert(
      "Input signature doesn't match up after de/serialization",
      memcmp(
        a_Tx->inputs[i].signature,
        other_tx->inputs[i].signature,
        SIGNATURE_LEN
      ) == 0
    );

    mbedtls_ecp_group group;
    mbedtls_ecp_group_init(&group);
    mbedtls_ecp_group_load(&group, CURVE);
    ser_pub_key(buf_1, a_Tx->inputs[i].pub_key, &group);
    ser_pub_key(buf_2, other_tx->inputs[i].pub_key, &group);
    mu_assert(
      "Input pub key doesn't match up after de/serialization",
      memcmp(buf_1, buf_2, PUB_KEY_SER_LEN) == 0
    );
  }

  mu_assert(
    "Outputs doesn't match after de/serialization",
    memcmp(a_Tx->outputs, other_tx->outputs, sizeof(Transaction)) == 0
  );

  unsigned char output_hash[TX_HASH_LEN];
  hash_tx(output_hash, a_Tx);
  unsigned char deser_output_hash[TX_HASH_LEN];
  hash_tx(deser_output_hash, other_tx);

  mu_assert(
    "Hashing isn't consistent after de/serialization",
    memcmp(output_hash, deser_output_hash, TX_HASH_LEN) == 0
  );

  return NULL;
}

static char  *test_ser_utxo() {
  UTXO *utxo, *desered_utxo;
  unsigned char *sered_utxo, *sered_utxo_2;

  utxo = _make_utxo();
  sered_utxo = ser_utxo_alloc(utxo);
  desered_utxo = deser_utxo_alloc(sered_utxo);

  mu_assert(
    "Amount isn't consistent after de-serialization",
    utxo->amt == desered_utxo->amt
  );
  mu_assert(
    "Public key hash isn't consistent after de-serialization",
    memcmp(
      utxo->public_key_hash,
      desered_utxo->public_key_hash,
      PUB_KEY_HASH_LEN
    ) == 0
  );

  // Ensure that we don't have padding bytes
  for (int i = 0; i < 10; i++) {
    sered_utxo_2 = ser_utxo_alloc(utxo);
    mu_assert(
      "Serialization of UTXOs isn't consistent",
      memcmp(sered_utxo, sered_utxo_2, size_ser_utxo()) == 0
    );
    free(sered_utxo_2);
  }

  free(utxo);
  free(sered_utxo);
  free(desered_utxo);

  return NULL;
}

static char *all_tests() {
  mu_run_test(test_ser_tx);
  mu_run_test(test_ser_utxo);
  return NULL;
}

int main() {
    char *result = all_tests();
  if (result != NULL) {
    printf("%s\n", result);
  } else {
    printf("ser_tx.c passing!\n");
  }
  printf("Tests run: %d\n", tests_run);

  return result != 0;
}
