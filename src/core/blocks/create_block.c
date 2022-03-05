#include "create_block.h"
#include "mempool.h"
/*
Get Number of transactions to try and include in block
*/
unsigned int calc_num_tx_target();


unsigned char get_prev_header_hash();
/*
What is the return type, are we just using an array of pointers?
*/
unsigned int get_txs_from_mempool(Transaction **tx_pts, unsigned int num_tx){
  unsigned int num_included = 0;
  MemPool *s;
  for(s = mempool; s != NULL; s = s->hh.next) {
    num_included++;
    *tx_pts = s->tx;
    if(num_included == num_tx){
      break;
    }
  }
  return num_included;
}

unsigned int calc_coinbase_reward(Transaction **tx_pts, unsigned int num_txs);

unsigned int calc_tx_fees();

Transaction *create_coinbase_tx();

unsigned int get_difficulty();

unsigned int get_new_nonce(unsigned int old_nonce);

BlockHeader *compile_header(unsigned int nonce);

Block *compile_block(BlockHeader *block_header, Transaction * txs);

int validate_header_hash(BlockHeader *block_header, unsigned int difficulty);

unsigned int get_difficulty();