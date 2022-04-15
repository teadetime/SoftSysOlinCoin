#pragma once
#include "constants.h"
#include "base_tx.h"

typedef struct BlockHeader{
  unsigned long timestamp;
  unsigned char all_tx[ALL_TX_HASH_LEN];
  unsigned char prev_header_hash[BLOCK_HASH_LEN];
  unsigned long nonce;
} BlockHeader;

typedef struct Block{
  unsigned int num_txs;
  BlockHeader header;
  Transaction** txs;
} Block;

/* Hashes passed block header
 *
 * dest: Buffer to write hash to, expected to be of size BLOCK_HASH_LEN
 * header: Block header to hash
 */
void hash_blockheader(unsigned char *dest, BlockHeader *header);

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

