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
#include "validate_tx.h"
#include "utxo_to_tx.h"
#include "double_spend_set.h"

void create_blank_sig_txhash(unsigned char *blank_hash, Transaction *tx){
  Transaction *blank_sig_tx = malloc(sizeof(Transaction));
  memcpy(blank_sig_tx, tx, sizeof(Transaction));

  blank_sig_tx->inputs = malloc(tx->num_inputs*sizeof(Input));
  blank_sig_tx->outputs = malloc(tx->num_outputs*sizeof(Output));

  memcpy(blank_sig_tx->inputs, tx->inputs, tx->num_inputs*sizeof(Input));

  // Copy the keys so that they aren't freed!
  for(unsigned int i = 0; i<tx->num_inputs; i++){
    blank_sig_tx->inputs[i].pub_key = malloc(sizeof(mbedtls_ecp_point));
    mbedtls_ecp_point_init(blank_sig_tx->inputs[i].pub_key);
    mbedtls_ecp_copy(
      blank_sig_tx->inputs[i].pub_key,
      tx->inputs[i].pub_key
    );
  }
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
  mbedtls_ecdsa_context new_key;
  build_ctx_from_public(&new_key, input->pub_key);
  if(validate_sig(input->signature, input->sig_len, blank_tx_hash,
                  TX_HASH_LEN, &new_key) != 0){
                    return 1;
                  }
  return 0;
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
  int input_matches_uxto = validate_input_matches_utxopool(input);
  int input_unlockable = check_input_unlockable(input, blank_tx_hash);
  if(input_matches_uxto != 0){
    return 1;
  }
  if(input_unlockable != 0){
    return 2;
  }
  return 0;
}

int validate_tx_parts_not_null(Transaction *tx){
  if(tx == NULL){
    return 1;
  }
  if(tx->num_inputs != 0){// Note 0 inputs happen for a coinbase tx and we expect inputs = Null
    if(tx->inputs == NULL){
      return 3;
    }
  }
  if(tx->num_outputs == 0){
    return 4;
  }
  if(tx->outputs == NULL){
    return 5;
  }
  return 0;
}

int validate_coinbase_tx_parts_not_null(Transaction *coinbase_tx){
  if(coinbase_tx == NULL){
    return 1;
  }
  if(coinbase_tx->num_inputs != 0){
    return 2;
  }
  if(coinbase_tx->inputs != NULL){
    return 3;
  }
  if(coinbase_tx->num_outputs != 1){
    return 4;
  }
  if(coinbase_tx->outputs == NULL){
    return 5;
  }
  return 0;
}
  
int validate_tx_shared(Transaction *tx){
  unsigned long total_in = 0;
  unsigned long total_out = 0;
  unsigned char blank_tx_hash[TX_HASH_LEN];
  if(validate_tx_parts_not_null(tx) != 0){
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
    if(tx->outputs->public_key_hash == NULL){
      return 3;
    }
  }
  if(total_in < total_out){
    return 4;
  }
  return 0;
}

int check_tx_double_spent(Transaction *tx){
  UTXOPool *double_spend_set = NULL;
  for(int i = 0; i < tx->num_inputs; i++){
    if(double_spend_add(double_spend_set, tx->inputs[i].prev_tx_id,
                        tx->inputs[i].prev_utxo_output) == 0){
      // Destroy Hashmap
      double_spend_delete(double_spend_set);
      return 1;
    }
  }
  return 0;
}

int check_inputs_not_in_mempool(Transaction *tx){
  unsigned char found_tx[TX_HASH_LEN];
  int found = 1;
  for(int i = 0; i<tx->num_inputs; i++){
    if(utxo_to_tx_find(found_tx, tx->inputs[i].prev_tx_id,
                     tx->inputs[i].prev_utxo_output) == 0){
      return 1;
    }
  }
  return 0;
}

int validate_tx_incoming(Transaction *tx){
  if(validate_tx_shared(tx)){
    return 1;
  }
  if(check_tx_double_spent(tx)){
    return 2;
  }
  if(check_inputs_not_in_mempool(tx)){
    return 3;
  }
  return 0;
}

