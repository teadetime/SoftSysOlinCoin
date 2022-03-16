#pragma once
#include "base_tx.h"
#include "base_block.h"
/*
Get Number of transactions to try and include in block
*/
unsigned int calc_num_tx_target();

unsigned char* get_prev_header_hash();

unsigned long calc_block_reward(unsigned long blockchain_height);


Transaction*  create_coinbase_tx(unsigned long tx_fees);

/*
Calculate tX fees for 1 tx
*/
unsigned int calc_tx_fees(Transaction* tx);


unsigned int get_txs_from_mempool(Transaction ***tx_pts);

unsigned char* hash_all_tx(Transaction** txs, unsigned int num_txs);

BlockHeader* create_block_header(Transaction** txs, unsigned int num_txs);


void create_block(Block* block);

Block* create_block_alloc();