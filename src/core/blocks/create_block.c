#include <stdlib.h>
#include <time.h>
#include "create_block.h"
#include "mempool.h"
#include "blockchain.h"
#include "utxo_pool.h"
/*
Get Number of transactions to try and include in block
*/
unsigned int calc_num_tx_target(){
  return DESIRED_NUM_TX;
}

unsigned int get_difficulty(){
  return HASH_DIFFICULTY;
}

unsigned char *get_prev_header_hash(){
  // Stored in globabls or a runtime Variable
  return top_block_header_hash;
}

unsigned long calc_block_reward(unsigned long blockchain_height){
  if(blockchain_height){}
  return BLOCK_REWARD;
}


Transaction *create_coinbase_tx(unsigned long tx_fees){
    // Create the Coinbase TX
  Transaction *coinbase_tx = malloc(sizeof(Transaction));
  Output *miner_output = malloc(sizeof(Output));
  miner_output->amt = tx_fees + calc_block_reward(chain_height);
  //TODO UPDATE BSED ON EAMONS PR TO GRAB A KEY AND PUT IT INTO THE KEYPOOL (ASSUMING MINER IS THE OWNER OF THIS WALLET)
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
unsigned int calc_tx_fees(Transaction *tx){
  unsigned long total_in = 0;
  unsigned long total_out = 0;
  
  for(unsigned int i = 0; i < tx->num_inputs; i++){
    UTXO* input_utxo = utxo_pool_find(tx->inputs[i].prev_tx_id, tx->inputs[i].prev_utxo_output);
    total_in += input_utxo->amt;
  }
  for(unsigned int i = 0; i < tx->num_outputs; i++){
    total_out += tx->outputs->amt;
  }
  return total_in - total_out;
}


unsigned int get_txs_from_mempool(Transaction ***tx_pts){
  unsigned long tx_fees = 0;
  unsigned int num_included = 0;
  MemPool *s;
  unsigned int tx_to_get = 1; // Start at one to include room for the coinbase the Coinbase TX

  unsigned int tx_desired = calc_num_tx_target();
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
  
  // Resize the Transaction memory if the wrong size was created
  if(num_included < tx_to_get){
    void *tmp = realloc(*tx_pts, num_included*sizeof(Transaction**));
    if (tmp == NULL) {
        //realloc didn't work  still points to the og location
    }
    else{
      *tx_pts = tmp;
    }
  }
  return num_included;
}

unsigned char *hash_all_tx(Transaction **txs, unsigned int num_txs){
  unsigned char *total_hash = malloc(ALL_TX_HASH_LEN);
  unsigned char *temp_all = malloc(TX_HASH_LEN*num_txs);
  
  for(unsigned int i = 0; i < num_txs; i++){
    hash_tx(temp_all+i*TX_HASH_LEN, (txs)[i]);
  }

  hash_sha256(total_hash, temp_all, TX_HASH_LEN*num_txs);
  free(temp_all);
  return total_hash;
}

BlockHeader *create_block_header(Transaction **txs, unsigned int num_txs){
  BlockHeader *b_header = malloc(sizeof(BlockHeader));
  unsigned char *prev_header_hash = get_prev_header_hash();
  unsigned char *all_tx_hash = hash_all_tx(txs, num_txs);

  memcpy(b_header->prev_header_hash, prev_header_hash, ALL_TX_HASH_LEN); 
  memcpy(b_header->all_tx, all_tx_hash, ALL_TX_HASH_LEN); 
  b_header->nonce = 0;
  b_header->timestamp = time(NULL);
  
  return b_header;
}


void create_block(Block *block){
  //First get the txs for the block
  Transaction **tx_ptr = malloc(sizeof(Transaction**));
  block->num_txs = get_txs_from_mempool(&tx_ptr);
  block->header = *create_block_header(tx_ptr, block->num_txs);
  block->txs = tx_ptr; // This is broken because things aren't aligned in memory
}

Block *create_block_alloc(){
  Block *block = malloc(sizeof(Block));
  create_block(block);
  return block;
}

void change_nonce(Block *block){
  block->header.nonce += 1;
}

unsigned char *hash_header(BlockHeader *block_header){
  unsigned char *serialized_header = ser_blockheader_alloc(block_header);
  unsigned char *header_hash = malloc(BLOCK_HASH_LEN);
  hash_sha256(header_hash, serialized_header, sizeof(BlockHeader));
  free(serialized_header);
  return header_hash;
}

int validate_header_hash(BlockHeader *block_header){
  // Hash the header
  unsigned char *header_hash = hash_header(block_header);
  unsigned char validator[HASH_DIFFICULTY];
  memset(validator, 0, HASH_DIFFICULTY);
  int ret = memcmp(validator, header_hash, HASH_DIFFICULTY);
  free(header_hash);
  if(ret != 0){
    return 1; // Failure, bad hash
  }
  else{
    return 0; // Difficulty Proof of work met
  }
}
