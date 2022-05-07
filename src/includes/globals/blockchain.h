/**
 * @file blockchain.h
 * @author  Eamon Ito-Fisher (efisher@olin.edu.com)
 * @brief Core BlockChain Datastructure and functions
 * @version 0.1
 * @date 2022-05-06
 * 
 * @copyright Copyright (c) 2022
 */
#pragma once

#include "constants.h"
#include "uthash.h"
#include "base_block.h"
#include "constants.h"
#include "leveldb/c.h"

/**
 * @brief Top Hash that the node is working on
 */
unsigned char top_block_header_hash[BLOCK_HASH_LEN];

/**
 * @brief Number of blocks in this node's Blockchain
 */
unsigned long chain_height;

/**
 * @brief Path to local blockchain
 */
char *blockchain_path;

/**
 * @brief Database
 */
leveldb_t *blockchain_db;  // Level DB Database

/**
 * @brief Create or a blockchain,, populates blockchain_db
 * 
 * @param db_env DB environment to use (Test or prod)
 * @return int 0 if success, not zero otherwise
 */
int blockchain_init_leveldb(char *db_env);

/**
 * @brief Destroys Blockchain database and chain/top_hash files
 * 
 * @return int 0 if success, not zero otherwise
 */
int destroy_blockchain();

/**
 * @brief Creates a new block in the blockchain
 * 
 * @param block block to add
 * @return int 0 if successs, not zero if not
 */
int blockchain_add_leveldb(Block *block);

/**
 * @brief Find a block in the blockchain
 * 
 * @param found_block Pointer in which to malloc a block if found. 
 * Not allocated if block not found
 * @param block_hash hash of block to locate
 * @return int 0 if successs, not zero if not
 */
int blockchain_find_leveldb(Block **found_block, unsigned char *block_hash);

/**
 * @brief Rmeoves a block from the blockchain
 * 
 * @param block_hash hash of block to remove
 * @return int 0 if successs, not zero if not
 */
int blockchain_remove_leveldb(unsigned char *block_hash);

/**
 * @brief Count the number of elements in the blockchain
 * 
 * @param num_entries pointer to store count into
 * @return int 0 if success, not zero if not
 */
int blockchain_count(unsigned int *num_entries);

/*
Prints the entire blockchain Hashmap to stdout

prefix: string to put in front of all print commands used for tabbing structure
*/

/**
 * @brief Print the blockchain hashmap
 * 
 * @param prefix String to print in front of hashmap
 */
void print_blockchain_hashmap(char *prefix);

/**
 * @brief Print the blockchain with abbreviated data (prettier)
 * 
 */
void pretty_print_blockchain_hashmap();

/**
 * @brief Reads the top hash from the local file
 * NOTE: requires blockchain to be initialized
 * 
 * @param dest destination buffer to read the data into, allocateed 
 * by the caller of BLOCK_HASH_LEN
 * @return int 0 if success, otherwise not zero
 */
int read_top_hash(unsigned char *dest);

/**
 * @brief Write the top block hash to local file
 * NOTE: requires blockchain to be initialized
 * @return int 0 if success, otherwise not zero
 */
int write_top_hash();

/**
 * @brief Deltes TOP_HASH_FILE
 * 
 * @return int 0 if success, not zero otherwise
 */
int delete_top_hash();

/**
 * @brief Read the chain height from local file
 * NOTE: requires blockchain to be initialized
 * @return int chainheight if successfully read, -1 if failure reading
 */
int read_chain();

/**
 * @brief Write the chainheight to local file
 * NOTE: requires blockchain to be initialized
 * 
 * @return int 0 if write successful, not zero if failure
 */
int write_chain_height();

/**
 * @brief Deltes the Chain height file
 * 
 * @return int 0 if success, not zero if failure
 */
int delete_chain_height();