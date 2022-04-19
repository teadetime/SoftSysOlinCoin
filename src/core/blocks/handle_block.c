#include "mempool.h"
#include "blockchain.h"
#include "utxo_pool.h"
#include "wallet_pool.h"
#include "utxo_to_tx.h"
#include "validate_block.h"

void update_local_blockchain(Block *block){
  // NOTE: NOT DEALING WITH BRANCHES HERE This is left for branch resolution code here
  //blockchain_add(block);  // T\NOTE this increases chain height and sets new top header hash
  int ret_add = blockchain_add_leveldb(block);
  if(ret_add != 0){
    fprintf(stderr, "Blockchain Add Failed: \n");
    exit(1);
  }
}

void update_UTXO_pool_and_wallet_pool(Block *block){
  for(unsigned int i = 0; i < block->num_txs; i++){
    for(unsigned int j = 0; j < block->txs[i]->num_outputs; j++){

      utxo_pool_add_leveldb(block->txs[i], j);
      mbedtls_ecdsa_context *keypair = check_if_output_unlockable_leveldb(block->txs[i], j);
      if(keypair != NULL){
        wallet_pool_build_add_leveldb(block->txs[i], j, keypair);
      }
    }
    for(unsigned int k = 0; k < block->txs[i]->num_inputs; k++){
      utxo_pool_remove_leveldb(block->txs[i]->inputs[k].prev_tx_id,
        block->txs[i]->inputs[k].prev_utxo_output);
      wallet_pool_remove_leveldb(block->txs[i]->inputs[k].prev_tx_id,
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

      if (ret == 0) {
        tx = mempool_remove(found_tx_hash);
        free_tx(tx);
      }
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
