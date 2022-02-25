#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "base_tx.h"


int main() {

  Output *an_Output = malloc(sizeof(Output));
  an_Output->amt = 5;
  strcpy(an_Output->public_key_hash, "a_val");

  Input *an_Input = malloc(sizeof(Input));
  an_Input->prev_utxo_output = 2;

  print_input(an_Input);
  print_output(an_Output);

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

  print_tx(a_Tx);

  // Other Access ways
  // printf("prev_tx_id[0]: %s\n", a_Tx->inputs->prev_tx_id);
  // printf("prev_tx_id[1]: %s\n", a_Tx->inputs[1].prev_tx_id);

  // printf("Output Amount: %li\n", a_Tx->outputs[0].amt);
  // printf("Output PubKey: %s\n", a_Tx->outputs[0].public_key_hash);
  // //printf("prev_tx_id: %s\n", ((a_Tx->inputs)+1)->prev_tx_id);

  //Serialization Testing
  char *char_tx = ser_tx_alloc(a_Tx);

  Transaction *other_tx = deser_tx(char_tx);

  print_tx(other_tx);
  return 0;
}
