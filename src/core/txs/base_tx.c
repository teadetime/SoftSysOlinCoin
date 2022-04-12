#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "base_tx.h"
#include "ser_tx.h"
#include "crypto.h"

void hash_tx(unsigned char *dest, Transaction *tx) {
  unsigned char *tx_buf;
  int tx_buf_size;

  tx_buf_size = size_tx(tx);
  tx_buf = ser_tx_alloc(tx);
  hash_sha256(dest, tx_buf, tx_buf_size);
  free(tx_buf);
}

int size_input() {
  return sizeof(Input) - sizeof(mbedtls_ecp_point*) + PUB_KEY_SER_LEN;
}

int size_tx(Transaction *tx){
  return (sizeof(tx->num_inputs) + sizeof(tx->num_outputs) +
    tx->num_inputs * size_input() + tx->num_outputs * sizeof(Output));
}

void print_input(Input *input, char *prefix){
  char *sub_prefix = malloc(strlen(prefix)+strlen(PRINT_TAB)+1);
  strcpy(sub_prefix, prefix);
  strcat(sub_prefix, PRINT_TAB);
  printf("%sInput Data  Sizeof(%li):\n", prefix, sizeof(Input));
  //printf("%sSignature: %s\n", sub_prefix, input->signature);
  dump_buf(sub_prefix, "signature:", input->signature, SIGNATURE_LEN);
  printf("%ssig_len: %li\n", sub_prefix, input->sig_len);
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
  dump_buf(sub_prefix, "public_key_hash:", output->public_key_hash, PUB_KEY_HASH_LEN);
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

void free_tx(Transaction *tx){
  if(tx != NULL){
    if(tx->inputs != NULL){
      for(unsigned int i = 0; i < tx->num_inputs; i++ ){
        if(tx->inputs[i].pub_key != NULL){
          mbedtls_ecp_point_free(tx->inputs[i].pub_key);
        }
      }
      free(tx->inputs);
    }
    if(tx->outputs != NULL){
      free(tx->outputs);
    }
  free(tx);
  }
}
void pretty_print_tx(Transaction *tx, char *prefix){
  unsigned char tx_hash[TX_HASH_LEN];

  hash_tx(tx_hash, tx);
  dump_buf(prefix, "Transaction Hash: ", tx_hash, TX_HASH_LEN);
  printf(SOFT_LINE_BREAK);

  printf("%s%i Input%s:\n", prefix, tx->num_inputs, (tx->num_inputs == 1 ? "" : "s"));
  printf(SOFT_LINE_BREAK);
  for(unsigned int i = 0; i < tx->num_inputs; i++){
    printf("%s  Output Index: %u\n", prefix, tx->inputs[i].prev_utxo_output);
    dump_buf(prefix, "  Source Tx: ", tx->inputs[i].prev_tx_id, TX_HASH_LEN);
    printf(SOFT_LINE_BREAK);
  }

  printf("%s%i Output%s:\n", prefix, tx->num_outputs, (tx->num_outputs > 1 ? "s" : ""));
  printf(SOFT_LINE_BREAK);
  for(unsigned int i = 0; i < tx->num_outputs; i++){
    printf("%s  Amount: %lu\n", prefix, tx->outputs[i].amt);
    dump_buf(prefix, "  Address: ", tx->outputs[i].public_key_hash, PUB_KEY_HASH_LEN);
    printf(SOFT_LINE_BREAK);
  }
}
