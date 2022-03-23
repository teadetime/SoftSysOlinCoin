/**
* @file validate_block.h
* @author Nathan Faber (nfaber@olin.edu.com)
* @brief Header for fof functions used to validate a new block that is 
 * broadcast on the blockchain and update the various appropriate structures
* @version 0.1
* @date 2022-03-18
* 
*/

#pragma once
#include "base_tx.h"
#include "base_block.h"
/*
How do we store branches or do we not worry right now, 
seems like a list would be okay but it's really a tree data structure?
*/


// WILL BE WRITTEN IN A SEPARATE PR by EAMON?
int validate_tx(Transaction *tx);

int validate_coinbase_tx(Transaction **txs, unsigned int num_txs);

int validate_txs(Transaction **txs, unsigned int num_txs);
///OPTIONAL
void request_prev_block(unsigned char *prev_header);

void add_to_pending_blocks(unsigned char *curr_header);

int validate_prev_block_exists(Block *block);

int validate_all_tx_hash(Block *block);

int validate_block(Block *block);

void update_local_blockchain(Block *block);

void update_UTXO_pool_and_wallet_pool(Block *block);

void update_mempool(Block *block);

void accept_block(Block *block);

void handle_new_block(Block *block);