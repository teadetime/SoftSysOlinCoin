/**
* @file handle_block.h
* @author Nathan Faber (nfaber@olin.edu.com)
* @brief Functions to "handle" a Block after it has been validated
* @version 0.1
* @date 2022-03-17
*/

#pragma once

#include "base_block.h"

/**
 * @brief Updates the local blockchain with a block
 *
 * Adds the block to the blockchain, then updates the block height and top block
 * hash
 *
 * @param block Block to update blockchain with
 */
void update_local_blockchain(Block *block);

/**
 * @brief Updates UTXO and wallet pool with a block
 *
 * Removes all block transaction inputs from the UTXO pool, then adds all block
 * transaction outputs to the UTXO pool. Also removed / adds UTXOs to the key
 * pool if the UTXOs are controlled by the key pool
 *
 * @param block Block to update utxo and wallet pool with
 */
void update_UTXO_pool_and_wallet_pool(Block *block);

/**
 * @brief Updates the mempool with a block
 *
 * Removes transactions from mempool that reference a UTXO spent by new block
 *
 * @param block Block to update mempool with
 */
void update_mempool(Block *block);

/**
 * @brief Updates data structures based on block
 *
 * @param block Block to update data structures with
 */
void accept_block(Block *block);

/**
 * @brief Validates block then updates data structures
 *
 * @param block Block to validate and update data structures with
 */
void handle_new_block(Block *block);
