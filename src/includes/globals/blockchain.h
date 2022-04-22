#pragma once

#include "constants.h"
#include "uthash.h"
#include "base_block.h"
#include "constants.h"
#include "leveldb/c.h"

typedef struct BlockChain {
  unsigned char id[BLOCK_HASH_LEN];
  Block *block;
  UT_hash_handle hh;
} BlockChain;

BlockChain *blockchain;
unsigned char top_block_header_hash[BLOCK_HASH_LEN];
unsigned long chain_height;

char *blockchain_path;
leveldb_t *blockchain_db;  // Level DB Database

/* Initializes the global blockchain variables */
int blockchain_init_leveldb(char *db_env);

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
void print_blockchain_hashmap(char *prefix);
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
int write_chain();