#pragma once
#include "constants.h"
#include "base_tx.h"

typedef struct BlockHeader{
  unsigned long timestamp;
  unsigned char all_tx[TX_HASH_LEN];
  unsigned char prev_header_hash[BLOCK_HASH_LEN];
  unsigned long nonce;
} BlockHeader;

typedef struct Block{
  unsigned int num_txs;
  BlockHeader header;
  Transaction** txs;
} Block;

/*
returns a pointer to end of memory of a serialized blockheader buffer(dest) used
for hashing, storage, or transmission

dest: buffer to write serialization into, must be sizeof(Block_header)
block_header: block header ot serialize
*/
unsigned char *ser_blockheader(unsigned char *dest, BlockHeader *block_header);

/*
returns a pointer to start of memory containing serialization of a block header 
allocates new memory, freeing is responsibility of caller

block_header: block_header to be serialized
*/
unsigned char *ser_blockheader_alloc(BlockHeader *block_header);

/* Hashes passed block header
 *
 * dest: Buffer to write hash to, expected to be of size BLOCK_HASH_LEN
 * header: Block header to hash
 */
void hash_blockheader(unsigned char *dest, BlockHeader *header);

/*
Returns size of block including size of variable inputs and outputs that
are malloced separately. Useful for serialization buffer creation

block: Block of which to get the size
*/
int size_block(Block *block);

/*
Returns pointer to end of dest buffer that contains the serialization of a block

dest: buffer to write the block serialization into, must be of size_block(block)
block: block to be serialized
*/
unsigned char *ser_block(unsigned char *dest, Block *block);

/*
Serializes a block and returns a pointer to the beginning of the memory where 
that lives, it is of length size_block(block) long

block: block to serialize
*/
unsigned char *ser_block_alloc(Block *block);

/*
Prints a Block Header to stdout so data can be visualized

header: header to be printed

*/
void print_block_header(BlockHeader *header, char *prefix);
void pretty_print_block_header(BlockHeader *block, char *prefix);

/*
Prints a block to stdout, (including header) for visualization

block: block to print
prefix: string to put in front of all print commands used for tabbing structure
*/
void print_block(Block *block, char *prefix);
void pretty_print_block(Block *block, char *prefix);

