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

#include "base_block.h"

void update_local_blockchain(Block *block);

void update_UTXO_pool_and_wallet_pool(Block *block);

void update_mempool(Block *block);

void accept_block(Block *block);

void handle_new_block(Block *block);
