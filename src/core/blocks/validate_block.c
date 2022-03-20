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
#include "create_block.h"
#include "mempool.h"
#include "blockchain.h"
#include "utxo_pool.h"
#include "wallet_pool.h"

// WILL BE WRITTEN IN A SEPARATE PR by EAMON?
int validate_tx(Transaction *tx){
  if(tx){}
  return 0;
}

int validate_coinbase_tx(Transaction *coinbase_tx, Transaction **txs, unsigned int num_txs){
  unsigned int calculated_total_fees = 0;
  // Note starting at 1 skips the coinbase tx
  for(unsigned int i = 1; i < num_txs; i++){
    calculated_total_fees += calc_tx_fees(txs[i]);
  }
  if(coinbase_tx->num_inputs != 0){
    return 1;
  }
  if(coinbase_tx->num_outputs != 1){
    return 2;
  }
  if(coinbase_tx->outputs == NULL){
    return 3;
  }
  if(calculated_total_fees + calc_block_reward(chain_height) != coinbase_tx->outputs[0].amt){
    return 4;
  }

  return 0;
}

int validate_txs(Transaction **txs, unsigned int num_txs){
  if(validate_coinbase_tx(txs[0], txs, num_txs) != 0){
    return 1;
  }
  for(unsigned int i = 1; i < num_txs; i++){
    if(validate_tx(txs[i]) != 0){
      return 2;
    }
  }
  return 0;
}
///OPTIONAL
void request_prev_block(unsigned char *prev_header){
  if(prev_header){}
  //Add the previous missiing header hash somewhere
}

void add_to_pending_blocks(unsigned char *curr_header){
  if(curr_header){}
  // Add to the blocks that need to be validated once we have teh previous block
}

int validate_prev_block_exists(Block *block){
  // Also check if it's in blockchain already....
  // Check if prev block is our latest top block

  Block *prev_block = blockchain_find(block->header.prev_header_hash);
  if(prev_block == NULL){
    add_to_pending_blocks(block->header.prev_header_hash);
    request_prev_block(block->header.prev_header_hash);
    return 1;
  }

  if(memcmp(block->header.prev_header_hash, top_block_header_hash, BLOCK_HASH_LEN) != 0 ){
    // NOTE this means we already have atleast one block ahead
    return 2;
  }
  return 0;
}

int validate_all_tx_hash(Block *block){
  unsigned char calculated_all_tx_hash[ALL_TX_HASH_LEN];
  hash_all_tx(calculated_all_tx_hash, block->txs, block->num_txs);
  return (memcmp(block->header.all_tx, calculated_all_tx_hash, ALL_TX_HASH_LEN) != 0);
}

int validate_block(Block *block){
  int valid_hash = try_header_hash(&(block->header));
  if(valid_hash != 0){
    return 1;
  }

  int valid_prev_block = validate_prev_block_exists(block);
  if(valid_prev_block != 0){
    return 2;
  }

  int valid_all_tx_hash = validate_all_tx_hash(block);
  if(valid_all_tx_hash != 0){
    return 3;
  }

  int valid_txs = validate_txs(block->txs, block->num_txs);
  if(valid_txs != 0){
    return 4;
  }

  return 0;
}

void update_local_blockchain(Block *block){
  // NOTE: NOT DEALING WITH BRANCHES HERE
  hash_blockheader(top_block_header_hash, &(block->header));
  blockchain_add(block);  // T\NOTE this increases chain height and sets new top header hash
}

void update_UTXO_pool_and_wallet_pool(Block *block){
  for(unsigned int i = 0 ; i < block->num_txs; i++){
    for(unsigned int j = 0; j < block->txs[i]->num_outputs; j++){
      utxo_pool_add(block->txs[i], j); // Don't need to do anything with return
      mbedtls_ecdsa_context *keypair = check_if_output_unlockable(block->txs[i], j);
      if(keypair != NULL){
        wallet_pool_add(block->txs[i], j, keypair);
      }
    }
    for(unsigned int k = 0; k < block->txs[i]->num_inputs; k++){
      UTXO * spent_utxo = utxo_pool_remove(block->txs[i]->inputs[k].prev_tx_id,
        block->txs[i]->inputs[k].prev_utxo_output);
      free(spent_utxo);
      wallet_pool_remove(block->txs[i]->inputs[k].prev_tx_id, 
        block->txs[i]->inputs[k].prev_utxo_output);
    }
  }
}

void update_mempool(Block *block){
  //Remove txs from mempool
  unsigned char tx_hash_to_remove[TX_HASH_LEN];
  for(unsigned int i = 0; i < block->num_txs; i++){
    hash_tx(tx_hash_to_remove, block->txs[i]);
    Transaction *ret_tx = mempool_remove(tx_hash_to_remove);
    if(ret_tx == NULL){
      // This would be bad
    }
  }
}

void accept_block(Block *block){
  // ALl the thigns to do when the block is good
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