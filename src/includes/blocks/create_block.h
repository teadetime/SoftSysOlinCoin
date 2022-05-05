/**
* @file create_block.h
* @author Nathan Faber (nfaber@olin.edu.com)
* @brief Functions to handle Block creation for mining functionality as 
well as mining
* @version 0.1
* @date 2022-03-17
* 
*/

#pragma once
#include "base_tx.h"
#include "base_block.h"
/**
 * @brief Get the current number of desired transactions for within a block
 * 
 * @return unsigned int  of transactions to include
 */
unsigned int calc_num_tx_target();

/**
 * @brief Get difficulty (number of zeros desired in hash for proof-of-work
 * 
 * @return unsigned int num of zeros
 */
unsigned int get_difficulty();

/**
 * @brief Calculate the reward for mining this block
 * 
 * @param blockchain_height not used(static block reward)
 * @return unsigned long reward in olincoins
 */
unsigned long calc_block_reward(unsigned long blockchain_height);

/**
 * @brief Create coinbase transaction by adding the block reward and the fees
 * @param total_tx_fees cumulative fees for the block
 * @return Transaction* to a new coinbase tx, freeing responsibility of caller
 */
Transaction *create_coinbase_tx(unsigned long total_tx_fees);

/**
 * @brief Calculate the transaction fees from a single tx
 * 
 * @param tx Transaction to calculate the fees of
 * @return unsigned int tx_fee in olincoins
 */
unsigned int calc_tx_fees(Transaction *tx);

/**
 * @brief Collect Transactions for a new block, including coinbase tx
 * 
 * @param tx_pts return destination of set of transactions
 * @return unsigned int number of transactions included
 */
unsigned int get_txs_from_mempool(Transaction ***tx_pts);

/**
 * @brief Create the hash of all the transaction hashes, used in block header
 * 
 * @param txs set of txs to hash
 * @param num_txs number in set of transactions
 * @return unsigned char* cumulative hash of 32 bytes (from mbed)
 */
void hash_all_tx(unsigned char *dest, Transaction **txs, unsigned int num_txs);

/**
 * @brief Create block header from a set of transactions
 * 
 * @param block block to write block header into
 * @param txs txs to use for the block/header
 * @param num_txs number of txs in txs
 */
void create_block_header(BlockHeader *header, Transaction **txs, unsigned int num_txs);

/**
 * @brief Creats a block header and allocates memory for it
 *
 * @param txs txs to use for the block/header
 * @param num_txs number of txs in txs
 * @return New blockheader
 */
BlockHeader *create_block_header_alloc(Transaction **txs, unsigned int num_txs);

/**
 * @brief Create a block from mempool to then be mined
 * 
 * @param block destination for created block
 */
void create_block(Block *block);

/**
 * @brief Creates a block and allocates the memory for it
 * 
 * @return Block* new block ready to mine, freeing responsibility of caller
 */
Block *create_block_alloc();

/**
 * @brief increments nonce of a block to try hashing/mining again
 * 
 * @param block block to increment the nonce on
 */
void change_nonce(Block *block);

/**
 * @brief Hash header and compare against difficulty, 
 * determine if Proof-of-Work is met
 * 
 * @param block_header block header to try hashing/comparing
 * @return int 0 if proof-of-work met, 1 if not
 */
int try_header_hash(BlockHeader *block_header);

/**
 * @brief Try mining a block
 * 
 * @param block block to mine
 * @return int 0 if proof-of-work met, 1 if not
 */
Block *mine_block();
