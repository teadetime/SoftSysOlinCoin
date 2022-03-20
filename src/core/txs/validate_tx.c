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
#include "constants.h"
#include "mbedtls/ecdsa.h"
#include "sign_tx.h"

int check_inputs_unlockable(Transaction *tx){
  unsigned char tx_hash[TX_HASH_LEN];

  mbedtls_ecdsa_context key;
  
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
  hash_tx(tx_hash, blank_sig_tx);
  free_tx(blank_sig_tx);
  for(unsigned int i = 0; i < tx->num_inputs; i++){
    mbedtls_ecdsa_init(&key);
    mbedtls_ecp_group_load(&key.private_grp, CURVE);
    key.private_Q = *tx->inputs[i].pub_key;
    int ret = mbedtls_ecdsa_read_signature(
          &key,
          tx_hash, TX_HASH_LEN,
          tx->inputs[i].signature, tx->inputs[i].sig_len
        );
    if(ret != 0){
      return 1;
    }
  }
  return 0;
}

int check_inputs_ref_UTXO(){
  return 1;
}

int validate_tx(Transaction *tx){
  if(tx){}
  return 0;
}