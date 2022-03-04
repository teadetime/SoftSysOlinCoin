#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "base_tx.h"

unsigned char *ser_utxo(UTXO *utxo){
  unsigned char *data = malloc(sizeof(UTXO));
  memcpy(data, &(utxo->amt), sizeof(utxo->amt));

  unsigned char *sig = data + sizeof(utxo->amt);
  memcpy(sig, &(utxo->public_key_hash), sizeof(utxo->public_key_hash));

  unsigned char *spent = sig + sizeof(utxo->public_key_hash);
  memcpy(spent, &(utxo->spent), sizeof(utxo->spent));

  return data;
}

UTXO *dser_utxo(unsigned char *data){
  UTXO *new_UTXO = malloc(sizeof(UTXO));

  // Stolen from https://cboard.cprogramming.com/cplusplus-programming/43180-sizeof-struct-member-problem.html
  memcpy(&(new_UTXO->amt), data, sizeof(((UTXO*)0)->amt));

  unsigned char *sig = data + sizeof(((UTXO*)0)->amt);
  memcpy(&(new_UTXO->public_key_hash), sig, sizeof(((UTXO*)0)->public_key_hash));

  unsigned char *spent = sig + sizeof(((UTXO*)0)->public_key_hash);
  memcpy(&(new_UTXO->spent), spent, sizeof(((UTXO*)0)->spent));

  return new_UTXO;
}

int size_tx(Transaction *tx){
  return (sizeof(tx->num_inputs) + sizeof(tx->num_outputs) +
    tx->num_inputs * sizeof(Input) + tx->num_outputs * sizeof(Output));
}

unsigned char *ser_tx(unsigned char *dest, Transaction *tx){
  memcpy(dest, &(tx->num_inputs), sizeof(tx->num_inputs));

  unsigned char *num_outputs = dest + sizeof(tx->num_inputs);
  memcpy(num_outputs, &(tx->num_outputs), sizeof(tx->num_outputs));

  unsigned char *inputs = num_outputs + sizeof(tx->num_outputs);
  memcpy(inputs, tx->inputs, tx->num_inputs * sizeof(Input));

  unsigned char *outputs = inputs + tx->num_inputs * sizeof(Input);
  memcpy(outputs, tx->outputs, tx->num_outputs * sizeof(Output));

  unsigned char * end = outputs + tx->num_outputs * sizeof(Output);
  return end;
}

unsigned char *ser_tx_alloc(Transaction *tx){
  unsigned char *data = malloc(size_tx(tx));
  ser_tx(data, tx);
  return data;
}

Transaction *deser_tx(unsigned char *data){
  Transaction *new_tx = malloc(sizeof(Transaction));

  memcpy(&(new_tx->num_inputs), data, sizeof(int));

  unsigned char *nm_outputs = data + sizeof(int);
  memcpy(&(new_tx->num_outputs), nm_outputs, sizeof(int));

  unsigned char *inputs = nm_outputs + sizeof(int);
  unsigned int input_sz = new_tx->num_inputs * sizeof(Input);
  new_tx->inputs = malloc(input_sz);
  memcpy(new_tx->inputs, inputs, input_sz);

  unsigned char *outputs = inputs + input_sz;
  unsigned int output_sz = new_tx->num_outputs * sizeof(Output);
  new_tx->outputs = malloc(output_sz);
  memcpy(new_tx->outputs, outputs, output_sz);

  return new_tx;
}

void hash_tx(unsigned char *dest, Transaction *tx) {
  unsigned char *tx_buf;
  int tx_buf_size;

  tx_buf_size = size_tx(tx);
  tx_buf = ser_tx_alloc(tx);
  hash_sha256(dest, tx_buf, tx_buf_size);
  free(tx_buf);
}

void print_input(Input *input, char *prefix){
  char *sub_prefix = malloc(strlen(prefix)+strlen(PRINT_TAB)+1);
  strcpy(sub_prefix, prefix);
  strcat(sub_prefix, PRINT_TAB);
  printf("%sInput Data  Sizeof(%li):\n", prefix, sizeof(Input));
  //printf("%sSignature: %s\n", sub_prefix, input->signature);
  dump_buf(sub_prefix, "signature:", input->signature, SIGNATURE_LEN);
  printf("%sprev_tx_id: %s\n", sub_prefix, input->prev_tx_id);
  printf("%sprev_output_num: %i\n", sub_prefix, input->prev_utxo_output);
  free(sub_prefix);
}

void print_output(Output *output, char *prefix){
  char *sub_prefix = malloc(strlen(prefix)+strlen(PRINT_TAB)+1);
  strcpy(sub_prefix, prefix);
  strcat(sub_prefix, PRINT_TAB);
  printf("%sOutput Data Sizeof(%li):\n", prefix, sizeof(Output));
  printf("%samt: %li\n", sub_prefix, output->amt);
  dump_buf(sub_prefix, "public_key_hash:", output->public_key_hash, TX_HASH_LEN);
  free(sub_prefix);
}

void print_tx(Transaction *tx, char *prefix){
  char *sub_prefix = malloc(strlen(prefix)+strlen(PRINT_TAB)+1);
  strcpy(sub_prefix, prefix);
  strcat(sub_prefix, PRINT_TAB);
  printf("%sTransaction Sizeof(%li), full_size(%i):\n", prefix, sizeof(*tx), size_tx(tx));
  printf("%snum_inputs: %i\n", sub_prefix, tx->num_inputs);
  printf("%snum_outputs: %i\n", sub_prefix, tx->num_outputs);
  printf("%sInputs\n", sub_prefix);
  for(unsigned int i = 0; i < tx->num_inputs; i++){
    print_input(&(tx->inputs[i]), sub_prefix);
  }
  printf("%sOutputs\n", sub_prefix);
  for(unsigned int i = 0; i < tx->num_outputs; i++){
    print_output(&(tx->outputs[i]), sub_prefix);
  }
  free(sub_prefix);
}
