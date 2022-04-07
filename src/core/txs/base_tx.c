#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "base_tx.h"
#include "sign_tx.h"
#include "utxo_pool.h"

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

int size_input() {
  return sizeof(Input) - sizeof(mbedtls_ecp_point*) + PUB_KEY_SER_LEN;
}

unsigned char *ser_input(unsigned char *dest, Input *input) {
  mbedtls_ecp_group group;

  mbedtls_ecp_group_init(&group);
  mbedtls_ecp_group_load(&group, CURVE);
  ser_pub_key(dest, input->pub_key, &group);

  unsigned char *sig_len = dest + PUB_KEY_SER_LEN;
  memcpy(sig_len, &(input->sig_len), sizeof(size_t));

  unsigned char *sig = sig_len + sizeof(size_t);
  memcpy(sig, input->signature, SIGNATURE_LEN);

  unsigned char *prev_tx = sig + SIGNATURE_LEN;
  memcpy(prev_tx, input->prev_tx_id, TX_HASH_LEN);

  unsigned char *vout = prev_tx + TX_HASH_LEN;
  memcpy(vout, &(input->prev_utxo_output), sizeof(unsigned int));

  return vout + sizeof(unsigned int);
}

unsigned char *deser_input(Input *dest, unsigned char *src) {
  mbedtls_ecp_group group;

  mbedtls_ecp_group_init(&group);
  mbedtls_ecp_group_load(&group, CURVE);
  dest->pub_key = malloc(sizeof(mbedtls_ecp_point));
  mbedtls_ecp_point_init(dest->pub_key);
  deser_pub_key(dest->pub_key, &group, src);

  unsigned char *sig_len = src + PUB_KEY_SER_LEN;
  memcpy(&(dest->sig_len), sig_len, sizeof(size_t));

  unsigned char *sig = sig_len + sizeof(size_t);
  memcpy(dest->signature, sig, SIGNATURE_LEN);

  unsigned char *prev_tx = sig + SIGNATURE_LEN;
  memcpy(dest->prev_tx_id, prev_tx, TX_HASH_LEN);

  unsigned char *vout = prev_tx + TX_HASH_LEN;
  memcpy(&(dest->prev_utxo_output), vout, sizeof(unsigned int));

  return vout + sizeof(unsigned int);
}

int size_tx(Transaction *tx){
  return (sizeof(tx->num_inputs) + sizeof(tx->num_outputs) +
    tx->num_inputs * size_input() + tx->num_outputs * sizeof(Output));
}

unsigned char *ser_tx(unsigned char *dest, Transaction *tx){
  memcpy(dest, &(tx->num_inputs), sizeof(tx->num_inputs));

  unsigned char *num_outputs = dest + sizeof(tx->num_inputs);
  memcpy(num_outputs, &(tx->num_outputs), sizeof(tx->num_outputs));

  unsigned char *inputs = num_outputs + sizeof(tx->num_outputs);
  for (size_t i = 0; i < tx->num_inputs; i++) {
    inputs = ser_input(inputs, &(tx->inputs[i]));
  }

  unsigned char *outputs = inputs;
  memset(outputs, 0, tx->num_outputs * sizeof(Output));
  memcpy(outputs, tx->outputs, tx->num_outputs * sizeof(Output));

  unsigned char *end = outputs + tx->num_outputs * sizeof(Output);
  return end;
}

unsigned char *ser_tx_alloc(Transaction *tx){
  unsigned char *data = malloc(size_tx(tx));
  ser_tx(data, tx);
  return data;
}

Transaction *deser_tx(unsigned char *data){
  Transaction *new_tx = malloc(sizeof(Transaction));

  memcpy(&(new_tx->num_inputs), data, sizeof(unsigned int));

  unsigned char *nm_outputs = data + sizeof(unsigned int);
  memcpy(&(new_tx->num_outputs), nm_outputs, sizeof(unsigned int));

  unsigned char *inputs = nm_outputs + sizeof(unsigned int);
  unsigned int input_sz = new_tx->num_inputs * sizeof(Input);
  new_tx->inputs = malloc(input_sz);
  for (size_t i = 0; i < new_tx->num_inputs; i++)
    inputs = deser_input(new_tx->inputs + i, inputs);

  unsigned char *outputs = inputs;
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
