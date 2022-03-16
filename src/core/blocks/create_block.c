#include "create_block.h"
#include "mempool.h"
#include "blockchain.h"
#include "utxo_pool.h"
#include <stdlib.h>
/*
Get Number of transactions to try and include in block
*/
unsigned int calc_num_tx_target(){
  return desired_num_tx;
}

unsigned char get_prev_header_hash(){
  // Stored in globabls or a runtime Variable
  return top_block_header_hash;
}

/*
What is the return type, are we just using an array of pointers?
*/
//Transaction **ret_txs;
//func(&ret_txs);
// ret_txs[0]->TXSTUFF
unsigned int get_txs_from_mempool(Transaction ***tx_pts){
  unsigned long tx_fees = 0;
  unsigned int num_included = 0;
  MemPool *s;
  unsigned int tx_to_get = 1; // Start at one to include room for the coinbase the Coinbase TX

  unsigned int tx_desired = calc_num_tx_target;
  unsigned int tx_in_mempool = HASH_COUNT(mempool);

  //Take minimum value of the mempool or the desired size
  tx_to_get += ((tx_desired) < (tx_in_mempool)) ? (tx_desired) : (tx_in_mempool);

  *tx_pts = malloc(tx_to_get*sizeof(Transaction**)); // Shouldn't this be multiplied by 4 because eeach pointer takes up 4 bytes?

  for(s = mempool; s != NULL; s = s->hh.next) {
    (*tx_pts)[num_included+1] = s->tx; // Leave the first tx for coinbase tx
    tx_fees += calc_tx_fees(s->tx);
    num_included++;
    if(num_included == tx_to_get){
      break;
    }
  }

  // Create the Coinbase TX
  (*tx_pts)[0] = create_coinbase_tx(tx_fees);
  
  return num_included;
}

unsigned long calc_block_reward(unsigned long blockchain_height){
  return block_reward;
}

Transaction*  create_coinbase_tx(unsigned long tx_fees){
    // Create the Coinbase TX
  Transaction* coinbase_tx = malloc(sizeof(Transaction));
  Output* miner_output = malloc(sizeof(Output));
  miner_output->amt = tx_fees + calc_block_reward(chain_height);
  memset(miner_output->public_key_hash, 0, PUB_KEY_HASH_LEN);
  //memcpy(miner_output->public_key_hash, , PUB_KEY_HASH_LEN)
  coinbase_tx->inputs = NULL;
  coinbase_tx->num_inputs = 0;
  coinbase_tx->num_outputs = 1;
  coinbase_tx->outputs = miner_output;

  return coinbase_tx;
}

/*
Calculate tX fees for 1 tx

*/
unsigned int calc_tx_fees(Transaction* tx){
  unsigned long total_in;
  unsigned long total_out;
  
  for(unsigned int i = 0; i < tx->num_inputs; i++){
    UTXO* input_utxo = utxo_pool_find(tx->inputs[i].prev_tx_id, tx->inputs[i].prev_utxo_output);
    total_in += input_utxo->amt;
  }
  for(unsigned int i = 0; i < tx->num_outputs; i++){
    total_out += tx->outputs->amt;
  }
  return total_in - total_out;
}

Transaction *create_coinbase_tx();

unsigned int get_difficulty();

unsigned int get_new_nonce(unsigned int old_nonce);

BlockHeader *compile_header(unsigned int nonce);

Block *compile_block(BlockHeader *block_header, Transaction * txs);

int validate_header_hash(BlockHeader *block_header, unsigned int difficulty);

unsigned int get_difficulty();