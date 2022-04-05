#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "minunit.h"
#include "base_tx.h"
#include "sign_tx.h"
#include "ser_tx.h"

int tests_run = 0;

static char  *test_tx_creation() {
  Output *an_Output = malloc(sizeof(Output));
  memset(an_Output, 0, sizeof(Output));
  an_Output->amt = 5;
  strcpy((char*)an_Output->public_key_hash, "a_val");

  Input *input_1 = malloc(sizeof(Input));
  memset(input_1, 0, sizeof(Input));
  input_1->prev_utxo_output = 2;
  mbedtls_ecdsa_context *key_pair_1 = gen_keys();
  input_1->pub_key = &key_pair_1->private_Q;

  Input *input_2 = malloc(sizeof(Input));
  memset(input_2, 0, sizeof(Input));
  input_2->prev_utxo_output = 4;
  mbedtls_ecdsa_context *key_pair_2 = gen_keys();
  input_2->pub_key = &key_pair_2->private_Q;

  Transaction *a_Tx = malloc(sizeof(Transaction));
  a_Tx->num_inputs = 2;
  a_Tx->num_outputs = 1;

  a_Tx->inputs = malloc(sizeof(Input) * (a_Tx->num_inputs));
  a_Tx->outputs = malloc(sizeof(Output) * (a_Tx->num_outputs));

  // Copy over some inputs
  memcpy(a_Tx->inputs, input_1, sizeof(*input_1) );
  memcpy(a_Tx->inputs + 1, input_2, sizeof(*input_2) );

  //Copy an Output
  memcpy(a_Tx->outputs, an_Output, sizeof(*an_Output) );

  //print_tx(a_Tx);

  // Other Access ways
  // printf("prev_tx_id[0]: %s\n", a_Tx->inputs->prev_tx_id);
  // printf("prev_tx_id[1]: %s\n", a_Tx->inputs[1].prev_tx_id);

  // printf("Output Amount: %li\n", a_Tx->outputs[0].amt);
  // printf("Output PubKey: %s\n", a_Tx->outputs[0].public_key_hash);
  // //printf("prev_tx_id: %s\n", ((a_Tx->inputs)+1)->prev_tx_id);

  //Serialization Testing
  unsigned char *char_tx = ser_tx_alloc(a_Tx);

  Transaction *other_tx = deser_tx(char_tx);

  unsigned char output_hash[TX_HASH_LEN];
  hash_tx(output_hash, a_Tx);
  unsigned char deser_output_hash[TX_HASH_LEN];
  hash_tx(deser_output_hash, other_tx);

  // Testing Section
  mu_assert(
    "Num Inputs doesn't match in serialization",
    a_Tx->num_inputs == other_tx->num_inputs
  );
  mu_assert(
    "Num Outputs doesn't match in serialization",
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

  // Test outputs
  mu_assert(
    "Outputs doesn't match after de/serialization",
    memcmp(a_Tx->outputs, other_tx->outputs, sizeof(Transaction)) == 0
  );

  // Test hashing
  mu_assert(
    "Hashing isn't consistent after serialization",
    memcmp(output_hash, deser_output_hash, TX_HASH_LEN) == 0
  );
  return NULL;
}

static char *all_tests() {
  mu_run_test(test_tx_creation);
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
