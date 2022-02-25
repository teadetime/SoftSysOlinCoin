#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "base_tx.h"

char *ser_utxo(UTXO *utxo){
  char *data = malloc(sizeof(UTXO));
  memcpy(data, &(utxo->amt), sizeof(utxo->amt));

  char *sig = data + sizeof(utxo->amt);
  memcpy(sig, &(utxo->signature), sizeof(utxo->signature));

  char *spent = sig + sizeof(utxo->signature);
  memcpy(spent, &(utxo->spent), sizeof(utxo->spent));

  return data;
}

UTXO *dser_utxo(char *data){
  UTXO * new_UTXO = malloc(sizeof(UTXO));

  // Stolen from https://cboard.cprogramming.com/cplusplus-programming/43180-sizeof-struct-member-problem.html
  memcpy(&(new_UTXO->amt), data, sizeof(((UTXO*)0)->amt));

  char * sig = data + sizeof(((UTXO*)0)->amt);
  memcpy(&(new_UTXO->signature), sig, sizeof(((UTXO*)0)->signature));

  char * spent = sig + sizeof(((UTXO*)0)->signature);
  memcpy(&(new_UTXO->spent), spent, sizeof(((UTXO*)0)->spent));

  return new_UTXO;
}

int size_tx(Transaction *tx){
    return (sizeof(tx->num_inputs) + sizeof(tx->num_outputs) +
      tx->num_inputs * sizeof(Input) + tx->num_outputs * sizeof(Output) + sizeof(char));
}

char *ser_tx( char *dest, Transaction *tx){
  memcpy(dest, &(tx->num_inputs), sizeof(tx->num_inputs));

  char *num_outputs = dest + sizeof(tx->num_inputs);
  memcpy(num_outputs, &(tx->num_outputs), sizeof(tx->num_outputs));

  char *inputs = num_outputs + sizeof(tx->num_outputs);
  memcpy(inputs, tx->inputs, tx->num_inputs * sizeof(Input));

  char *outputs = inputs + tx->num_inputs * sizeof(Input);
  memcpy(outputs, tx->outputs, tx->num_outputs * sizeof(Output));

  char * end = outputs + tx->num_outputs * sizeof(Output);
  return end;
}

char *ser_tx_alloc(Transaction *tx){
  char *data = malloc(size_tx(tx));
  ser_tx(data, tx);
  return data;
}

Transaction *deser_tx(char *data){
  Transaction *new_tx = malloc(sizeof(Transaction));

  memcpy(&(new_tx->num_inputs), data, sizeof(int));

  char *nm_outputs = data + sizeof(int);
  memcpy(&(new_tx->num_outputs), nm_outputs, sizeof(int));

  char *inputs = nm_outputs + sizeof(int);
  int input_sz = new_tx->num_inputs * sizeof(Input);
  new_tx->inputs = malloc(input_sz);
  memcpy(new_tx->inputs, inputs, input_sz);

  char *outputs = inputs + input_sz;
  int output_sz = new_tx->num_outputs * sizeof(Output);
  new_tx->outputs = malloc(output_sz);
  memcpy(new_tx->outputs, outputs, output_sz);

  return new_tx;
}

void print_input(Input *input){
  printf("\nInput Data:\n");
  printf("sig: %s\n", input->signature);
  printf("prev_tx_id: %s\n", input->prev_tx_id);
  printf("prev_output_num: %i\n", input->prev_utxo_output);
  printf("Size of Input(Bytes): %li\n", sizeof(Input));
}

void print_output(Output *output){
  printf("\nOutput Data:\n");
  printf("amt: %li\n", output->amt);
  printf("private Key: %s\n", output->public_key_hash);
  printf("Size Output(Bytes): %li\n", sizeof(Output));

}

void print_tx(Transaction *tx){
  printf("\nTransaction Data:\n");
  printf("num_inputs: %i\n", tx->num_inputs);
  printf("num_outputs: %i\n", tx->num_outputs);

  for(int i = 0; i < tx->num_inputs; i++){
    print_input(&(tx->inputs[i]));
  }

  for(int i = 0; i < tx->num_outputs; i++){
    print_output(&(tx->outputs[i]));
  }
  printf("Tx(Bytes): %li\n", sizeof(*tx)); // Check i
}
