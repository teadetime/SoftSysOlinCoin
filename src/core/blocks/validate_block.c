#include "create_block.h"
#include "mempool.h"
#include "blockchain.h"
#include "utxo_pool.h"
#include "validate_block.h"
/*
How do we store branches or do we not worry right now, 
seems like a list would be okay but it's really a tree data structure?
*/


// WILL BE WRITTEN IN A SEPARATE SPOT
int validate_tx(Transaction *tx){
  return 0;
}
int validate_coinbase_tx(Transaction *tx){
  return 0;
}

int validate_prev_block_exists(Block *block){
  // Also check if it's in blockchain already....
  // Check if prev block is our latest top block

  int failure = 0; // 0 is successful
  Block *prev_block = blockchain_find(block->header.prev_header_hash);
  if(prev_block == NULL){
    failure = 1;
    return 1;
  }

  if(memcmp(block->header.prev_header_hash, get_prev_header_hash(), BLOCK_HASH_LEN) != 0 ){
    // Not the top which means we should get more blocks
    failure = 2;
    add_to_pending_blocks(block->header.prev_header_hash);
    request_prev_block(block->header.prev_header_hash);
    return 1;
  }

  // Check if block already in blockchain
  

  if(failure != 0){
    return 1;
  }
  else{
    return 0;
  }
}

int validate_all_tx_hash(Block *block){
  return 1;
}

int validate_block_header(Block *block){

}

int try_validate_block(Block *new_block){
  // Check if hash meets the difficulty using other function
  return mine_block(new_block);
}

void update_local_blockchain(Block *block){
  // Do we need to resolve branches here?
  //cahin heght
  //top block hash
  //add to hashmap
  chain_height += 1;
  hash_blockheader(top_block_header_hash, &(block->header));
  blockchain_add(block);
}

void update_UTXO_pool(Block *block){
  //Create new utxo
  //Remove old should we free them!?! or just put spent
}

void update_mempool(Block *block){
  //Remove txs from mempool
}

void update_wallet_pool(){
  // If utxo have been spent those should be removed
  //Does anyhting happent othe keypool
}

void accept_block(Block *block){
  // ALl the thigns to do when the block is good
  update_local_blockchain(block);
  update_UTXO_pool(block);
  update_mempool(block);
}

void handle_new_block(Block *block){
  int ret = try_validate_block(block);
  if(ret == 0){
    accept_block(block);
  }
  else{
  }
  //Wrapper for all behaviour witha new block
}
///OPTIONAL
void request_prev_block(unsigned char *prev_header){
  //Add the previous missiing header hash somewhere
}

void add_to_pending_blocks(unsigned char *prev_header){
  // Add to the blocks that need to be validated once we have teh previous block
}