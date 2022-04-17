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
#include "validate_block.h"
#include "create_block.h"
#include "mempool.h"
#include "blockchain.h"
#include "utxo_to_tx.h"
#include "double_spend_set.h"
#include "validate_tx.h"

///OPTIONAL
void request_prev_block(unsigned char *prev_header){
  if(prev_header){}
  //Add the previous missing header hash somewhere
}

void add_to_pending_blocks(unsigned char *curr_header){
  if(curr_header){}
  // Add to the blocks that need to be validated once we have the previous block
}

int validate_prev_block_exists(Block *block){
  // Also check if it's in blockchain already....
  // Check if prev block is our latest top block

  Block *prev_block = NULL;
  int ret_find = blockchain_find_leveldb(&prev_block, block->header.prev_header_hash);
  printf("Find return: %i", ret_find);
  if(prev_block == NULL || ret_find != 0){
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


int validate_coinbase_tx(Transaction **txs, unsigned int num_txs){
  if(validate_coinbase_tx_parts_not_null(txs[0]) != 0){
    return 1;
  }
  unsigned int calculated_total_fees = 0;
  // Note starting at 1 skips the coinbase tx
  for(unsigned int i = 1; i < num_txs; i++){
    calculated_total_fees += calc_tx_fees(txs[i]);
  }

  if(calculated_total_fees + calc_block_reward(chain_height) != txs[0]->outputs[0].amt){
    return 2;
  }

  return 0;
}

int validate_incoming_block_txs(Transaction **txs, unsigned int num_txs){
  for(unsigned int i = 1; i < num_txs; i++){
    if(validate_tx_shared(txs[i]) != 0){
      return 1;
    }
  }
  if(validate_coinbase_tx(txs, num_txs) != 0){
    return 2;
  }

  return 0;
}

int validate_block_double_spend(Block *block){
  UTXOPool *double_spend_set;

  double_spend_set_init(double_spend_set);
  for (unsigned int i = 0; i < block->num_txs; i++) {
    for (unsigned int j = 0; j < block->txs[i]->num_inputs; j++) {
      if (double_spend_add(
            double_spend_set,
            block->txs[i]->inputs[i].prev_tx_id,
            block->txs[i]->inputs[i].prev_utxo_output
      ) != 0) {
        delete_double_spend_set(double_spend_set);
        return 1;
      }
    }
  }
  delete_double_spend_set(double_spend_set);

  return 0;
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

  int valid_txs = validate_incoming_block_txs(block->txs, block->num_txs);
  if(valid_txs != 0){
    return 4;
  }

  int valid_double_spend = validate_block_double_spend(block);
  if(valid_txs != 0){
    return 4;
  }

  return 0;
}
