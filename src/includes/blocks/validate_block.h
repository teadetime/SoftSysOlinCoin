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

int validate_tx_shared(Transaction *tx);

int validate_coinbase_tx(Transaction **txs, unsigned int num_txs);

int validate_incoming_block_txs(Transaction **txs, unsigned int num_txs);

///OPTIONAL
void request_prev_block(unsigned char *prev_header);

void add_to_pending_blocks(unsigned char *curr_header);

int validate_prev_block_exists(Block *block);

int validate_all_tx_hash(Block *block);

int validate_block_double_spend(Block *block);

int validate_block(Block *block);
