/**
* @file validate_block.h
* @author Nathan Faber (nfaber@olin.edu.com)
* @brief Header for functions used to validate a new Block 
* @version 0.1
* @date 2022-03-18
* 
*/

#pragma once

#include "base_tx.h"
#include "base_block.h"
/**
 * @brief Validate a transaction both when inocming and in a block
 * 
 * @param tx Transaction to validate
 * @return int 0 if success, otherwise not zero
 */
int validate_tx_shared(Transaction *tx);

/**
 * @brief Validate a coinbase Transaction
 * 
 * @param txs set of transactions with the coinbase transaction first
 * @param num_txs number of transaction in the set
 * @return int 0 if valid, not zero if invalid
 */
int validate_coinbase_tx(Transaction **txs, unsigned int num_txs);

/**
 * @brief Validate transactions in an incoming block
 * 
 * @param txs Transactions within a block
 * @param num_txs number of transactions in block
 * @return int 0 if valid, not zero if invalid
 */
int validate_incoming_block_txs(Transaction **txs, unsigned int num_txs);

/**
 * @brief NOT IMPLEMENTED
 * 
 * @param prev_header 
 */
void request_prev_block(unsigned char *prev_header);

/**
 * @brief NOT IMPLEMENTED
 * 
 * @param curr_header 
 */
void add_to_pending_blocks(unsigned char *curr_header);

/**
 * @brief Validate if the block's previous header hash is in local blockchain
 * 
 * @param block Block to check 
 * @return int 0 if previous header is found, not zero if not
 */
int validate_prev_block_exists(Block *block);

/**
 * @brief Verify that the all_tx_hash matches the set of transactions in a block
 * 
 * @param block Block to check
 * @return int 0 if all_tx_hash is valid, not zero if invalid
 */
int validate_all_tx_hash(Block *block);

/**
 * @brief Check if a block conatins a double-spend
 * 
 * @param block Block to check
 * @return int 0 if no double spends, not zero if double spend present
 */
int validate_block_double_spend(Block *block);

/**
 * @brief Validate an entire block
 * 
 * @param block Block to validate
 * @return int 0 if block is valid, not zero if invalid
 */
int validate_block(Block *block);
