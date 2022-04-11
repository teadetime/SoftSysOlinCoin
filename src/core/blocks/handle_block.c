/**
 * @file validate_block.c
 * @author Nathan Faber nfaber@olin.edu
 * @brief implementation of functions used to validate a new block that is
 * broadcast on the blockchain and update the various appropriate structures
 * @version 0.1
 * @date 2022-03-20
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "mempool.h"
#include "blockchain.h"
#include "utxo_pool.h"
#include "wallet_pool.h"
#include "utxo_to_tx.h"
#include "validate_block.h"

void update_local_blockchain(Block *block){
  // NOTE: NOT DEALING WITH BRANCHES HERE This is left for branch resolution code here
  blockchain_add(block);  // T\NOTE this increases chain height and sets new top header hash
}

void update_UTXO_pool_and_wallet_pool(Block *block){
  for(unsigned int i = 0; i < block->num_txs; i++){
    for(unsigned int j = 0; j < block->txs[i]->num_outputs; j++){
      utxo_pool_add(block->txs[i], j); // Don't need to do anything with return
      mbedtls_ecdsa_context *keypair = check_if_output_unlockable(block->txs[i], j);
      if(keypair != NULL){
        wallet_pool_add(block->txs[i], j, keypair);
      }
    }
    for(unsigned int k = 0; k < block->txs[i]->num_inputs; k++){
      UTXO *spent_utxo = utxo_pool_remove(block->txs[i]->inputs[k].prev_tx_id,
        block->txs[i]->inputs[k].prev_utxo_output);
      free(spent_utxo);
      wallet_pool_remove(block->txs[i]->inputs[k].prev_tx_id,
        block->txs[i]->inputs[k].prev_utxo_output);
    }
  }
}

void update_mempool(Block *block){
  Transaction *tx;
  unsigned char found_tx_hash[TX_HASH_LEN];
  int ret;

  for (unsigned int i = 0; i < block->num_txs; i++) {
    for (unsigned int j = 0; j < block->txs[i]->num_inputs; j++) {
      ret = utxo_to_tx_find(
        found_tx_hash,
        block->txs[i]->inputs[j].prev_tx_id,
        block->txs[i]->inputs[j].prev_utxo_output
      );

      // UTXO not in mapping, don't have to do anything
      if (ret != 0)
        continue;

      // Remove tx from mempool
      tx = mempool_remove(found_tx_hash);
      // Remove every mapping that referenced removed tx
      for (unsigned int k = 0; k < tx->num_inputs; k++)
        utxo_to_tx_remove(
          tx->inputs[k].prev_tx_id,
          tx->inputs[k].prev_utxo_output
        );
      free_tx(tx);
    }
  }
}

void accept_block(Block *block){
  // All the things to do when the block is good
  update_local_blockchain(block);
  update_UTXO_pool_and_wallet_pool(block);
  update_mempool(block);
}

void handle_new_block(Block *block){
  int ret = validate_block(block);
  if(ret == 0){
    accept_block(block);
  }
  else{
    // Different messages here to determine if we need to request headers etc
    //Right now we are just accepting the block or rejecting, no in between
  }
}
