/**
* @file base_block.h
* @author Nathan Faber (nfaber@olin.edu.com)
* @brief Base Block definitions and print functions
* @version 0.1
* @date 2022-03-17
* 
*/

#pragma once
#include "constants.h"
#include "base_tx.h"

/**
 * @brief Blockheader for Block in the BlockChain
 * 
 */
typedef struct BlockHeader{
  unsigned long timestamp;
  unsigned char all_tx[ALL_TX_HASH_LEN];
  unsigned char prev_header_hash[BLOCK_HASH_LEN];
  unsigned long nonce;
} BlockHeader;

/**
 * @brief Block within the BlockChain
 * 
 */
typedef struct Block{
  unsigned int num_txs;
  BlockHeader header;
  Transaction** txs;
} Block;

/**
 * @brief Hashes passed block header
 * 
 * @param dest Buffer to write hash to, expected to be of size BLOCK_HASH_LEN
 * @param header Block header to hash
 */
void hash_blockheader(unsigned char *dest, BlockHeader *header);

/**
 * @brief Print an entire Block Header to stdout so data can be visualized
 * 
 * @param header Header to be printed
 * @param prefix string prefix to print
 */
void print_block_header(BlockHeader *header, char *prefix);

/**
 * @brief Print a smaller/prettier version of the blockheader
 * 
 * @param header Header to be printed
 * @param prefix string prefix to print
 */
void pretty_print_block_header(BlockHeader *block, char *prefix);

/**
 * @brief Print a block to stdout, (including header) for visualization
 * 
 * @param block Block to print
 * @param prefix string to prefix print commands (for tabbing structure)
 */
void print_block(Block *block, char *prefix);

/**
 * @brief Print a smaller/prettier block to stdout, 
 * (including header) for visualization
 * 
 * @param block Block to print
 * @param prefix string to prefix print commands (for tabbing structure)
 */
void pretty_print_block(Block *block, char *prefix);

