#pragma once

#include "constants.h"
#include "uthash.h"
#include "base_block.h"

typedef struct BlockChain {
  unsigned char id[BLOCK_HASH_LEN];
  Block *block;
  UT_hash_handle hh;
} BlockChain;

BlockChain *blockchain;

/* Initializes the global blockchain variable */
void blockchain_init();

/* Creates a new entry in the hashmap with the passed block
 *
 * Returns the pointer to passed block if node created, NULL otherwise
 *
 * block: Pointer to block that will be stored in entry
 */
Block *blockchain_add(Block *block);

/* Removes the entry corresponding to header_hash
 *
 * Returns the block stored in removed entry if succesfully removed, NULL otherwise
 *
 * header_hash: Buffer of length BLOCK_HASH_LEN, hash of block header
 */
Block *blockchain_remove(unsigned char *header_hash);

/* Finds block corresponding to header_hash
 *
 * Returns block if found, NULL otherwise
 *
 * header_hash: Buffer of length BLOCK_HASH_LEN, hash of block header
 */
Block *blockchain_find(unsigned char *header_hash);


/* Finds entry corresponding to header_hash
 *
 * Returns entry if found, NULL otherwise
 *
 * header_hash: Buffer of length BLOCK_HASH_LEN, hash of block header
 */
BlockChain *blockchain_find_node(unsigned char *header_hash);
