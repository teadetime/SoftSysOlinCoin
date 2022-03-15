#include "create_block.h"
#include "mempool.h"
#include "blockchain.h"
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
  unsigned int tx_desired = calc_num_tx_target;
  unsigned int tx_in_mempool = HASH_COUNT(mempool);

  //Take minimum value
  unsigned int tx_to_get = 1; // Add the Coinbase TX
  tx_to_get += ((tx_desired) < (tx_in_mempool)) ? (tx_desired) : (tx_in_mempool);

  *tx_pts = malloc(tx_to_get*sizeof(Transaction**)); // Shouldn't this be multiplied by 4 because eeach pointer takes up 4 bytes?

  unsigned int num_included = 0;
  MemPool *s;
  for(s = mempool; s != NULL; s = s->hh.next) {
    num_included++;
    (*tx_pts)[num_included] = s->tx;
    if(num_included == tx_to_get){
      break;
    }
  }

  // optionally let's realloc the malloc? or not needed?
  return num_included;
}

unsigned int calc_coinbase_reward(Transaction ***tx_pts, unsigned int num_txs);

unsigned int calc_tx_fees();

Transaction *create_coinbase_tx();

unsigned int get_difficulty();

unsigned int get_new_nonce(unsigned int old_nonce);

BlockHeader *compile_header(unsigned int nonce);

Block *compile_block(BlockHeader *block_header, Transaction * txs);

int validate_header_hash(BlockHeader *block_header, unsigned int difficulty);

unsigned int get_difficulty();