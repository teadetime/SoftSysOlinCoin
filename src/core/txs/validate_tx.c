/**
 * @file validate_tx.c
 * @author Nathan Faber nfaber@olin.edu
 * @brief 
 * @version 0.1
 * @date 2022-03-20
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "base_tx.h"
#include "utxo_pool.h"
#include "mempool.h"
#include "constants.h"
#include "mbedtls/ecdsa.h"
#include "sign_tx.h"
#include "create_block.h"

void create_blank_sig_txhash(unsigned char *blank_hash, Transaction *tx){

  Transaction *blank_sig_tx = malloc(sizeof(Transaction));
  memcpy(blank_sig_tx, tx, sizeof(Transaction));

  blank_sig_tx->inputs = malloc(tx->num_inputs*sizeof(Input));
  blank_sig_tx->outputs = malloc(tx->num_outputs*sizeof(Output));

  memcpy(blank_sig_tx->inputs, tx->inputs, tx->num_inputs*sizeof(Input));
  memcpy(blank_sig_tx->outputs, tx->outputs, tx->num_outputs*sizeof(Output));
  
  for(unsigned int i = 0; i < blank_sig_tx->num_inputs; i++){
    blank_sig_tx->inputs[i].sig_len = 0;
    memset(blank_sig_tx->inputs[i].signature, 0, SIGNATURE_LEN);
  }
  // need to make hash where the tx has signatures that are 0's
  hash_tx(blank_hash, blank_sig_tx);
  free_tx(blank_sig_tx);
}

int check_input_unlockable(Input *input, unsigned char *blank_tx_hash){
  unsigned char test_pub_key_hash[PUB_KEY_HASH_LEN];
  mbedtls_ecdsa_context new_key;
  UTXO *old_utxo;
  //unsigned char blank_tx_hash[TX_HASH_LEN];
  // create_blank_sig_txhash(blank_tx_hash, tx);

  build_ctx_from_public(&new_key, input->pub_key);
  if(validate_sig(input->signature, input->sig_len, blank_tx_hash,
                  TX_HASH_LEN, &new_key) != 0){
                    return 1;
                  }
  return 0;
}

int validate_inputs_vs_outputs(Transaction *tx){
  // This functionality is extremely similar to calc_tx_fees....
  UTXO *old_utxo;
  unsigned int total_in = 0;
  unsigned int total_out = 0;
  for(unsigned int i = 0; i < tx->num_inputs; i++){
    // Make sure the referenced utxo exists and the pubilc key matches the hash
    old_utxo = utxo_pool_find(tx->inputs[i].prev_tx_id, tx->inputs[i].prev_utxo_output);
    if(old_utxo == NULL){
      return 1;
    }
    total_in += old_utxo->amt;
  }
  for(unsigned int i = 0; i < tx->num_outputs; i++){
    total_out += tx->outputs[i].amt;
  }

  if(total_in < total_out){
    return 2;
  }
  return 0;
}


int validate_input_not_in_mempool(Input *input){
  Transaction *mempool_tx;
  mempool_tx = mempool_find(input->prev_tx_id);
  if(mempool_tx != NULL){
    return 1;
  }
}

int validate_input_matches_utxopool(Input *input){
  mbedtls_ecdsa_context new_key;
  unsigned char test_pub_key_hash[PUB_KEY_HASH_LEN];
  build_ctx_from_public(&new_key, input->pub_key);
  hash_pub_key(test_pub_key_hash, &new_key);
  UTXO *old_utxo = utxo_pool_find(input->prev_tx_id, input->prev_utxo_output);
  if(old_utxo == NULL){
    return 1;
  }
  if(memcmp(old_utxo->public_key_hash, test_pub_key_hash, PUB_KEY_HASH_LEN) != 0){
    return 2;
  }
  return 0;
}

int validate_input(Input *input, unsigned char *blank_tx_hash){
  int not_in_mempool = validate_input_not_in_mempool(input);
  int input_matches_uxto = validate_input_matches_utxopool(input);
  int input_unlockable = check_input_unlockable(input, blank_tx_hash);
}

int validate_tx_not_null(Transaction *tx){
  if(tx->num_inputs != 0){
    return 1;
  }
  if(tx->inputs != NULL){
    return 2;
  }
  if(tx->num_outputs != 0){
    return 3;
  }
  if(tx->outputs != NULL){
    return 4;
  }
}

int validate_tx(Transaction *tx){
  unsigned long total_in = 0;
  unsigned long total_out = 0;
  unsigned char blank_tx_hash[TX_HASH_LEN];
  if(validate_tx_not_null(tx) != 0){
    return 1;
  }
  create_blank_sig_txhash(blank_tx_hash, tx);
  for(unsigned int i = 0; i < tx->num_inputs; i++){
    if(validate_input(&tx->inputs[i], blank_tx_hash) != 0){
      return 2;
    }
    // Check the inputs are larger than outputs
    UTXO* input_utxo = utxo_pool_find(tx->inputs[i].prev_tx_id, tx->inputs[i].prev_utxo_output);
    total_in += input_utxo->amt;  // input utxo is not null based on previous tests
  }

  for(unsigned int i = 0; i < tx->num_outputs; i++){
    total_out += tx->outputs->amt;
  }
  if(total_in < total_out){
    return 3;
  }
  return 0;
}