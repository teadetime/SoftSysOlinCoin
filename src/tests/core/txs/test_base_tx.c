#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "minunit.h"
#include "base_tx.h"

int tests_run = 0;

static char  *test_tx_creation() {
  Output *an_Output = malloc(sizeof(Output));
  an_Output->amt = 5;
  strcpy((char*)an_Output->public_key_hash, "a_val");

  Input *an_Input = malloc(sizeof(Input));
  an_Input->prev_utxo_output = 2;

  // print_input(an_Input);
  // print_output(an_Output);

  Transaction *a_Tx = malloc(sizeof(Transaction));
  a_Tx->num_inputs = 2;
  a_Tx->num_outputs = 1;

  a_Tx->inputs = malloc(sizeof(Input) * (a_Tx->num_inputs));
  a_Tx->outputs = malloc(sizeof(Output) * (a_Tx->num_outputs));

  // Copy over some inputs
  memcpy(a_Tx->inputs, an_Input, sizeof(*an_Input) );
  memcpy(a_Tx->inputs + 1, an_Input, sizeof(*an_Input) );

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
  mu_assert(
    "Inputs doesn't match after de/serialization",
    memcmp(a_Tx->inputs, other_tx->inputs, sizeof(Transaction)) == 0
  );
  mu_assert(
    "Outputs doesn't match after de/serialization",
    memcmp(a_Tx->outputs, other_tx->outputs, sizeof(Transaction)) == 0
  );
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
    printf("constants.c passing!\n");
  }
  printf("Tests run: %d\n", tests_run);

  return result != 0;
}
