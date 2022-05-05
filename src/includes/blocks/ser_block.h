#pragma once

#include "base_block.h"

/******************************************************************************
 * Blockheaders
 ******************************************************************************/

#define BLOCKHEADER_SER_LEN \
  ( \
    sizeof(((BlockHeader*)0)->timestamp) + \
    sizeof(((BlockHeader*)0)->all_tx) + \
    sizeof(((BlockHeader*)0)->prev_header_hash) + \
    sizeof(((BlockHeader*)0)->nonce) \
  )

/**
 * @brief Serialize a BlockHeader
 *
 * @param dest Buffer of length size_ser_blockheader() to write to
 * @param blockheader BlockHeader to serialize
 * @return Number of bytes written if succesfull, -1 otherwise
 */
ssize_t ser_blockheader(unsigned char *dest, BlockHeader *blockheader);

/**
 * @brief Allocates memory and serializes a BlockHeader
 *
 * @param written Stores number of bytes written if succesfull, -1 otherwise
 * @param blockheader BlockHeader to serialize
 * @return unsigned char* Serialized blockheader of length
 *   size_ser_blockheader() if succesfull, NULL otherwise
 */
unsigned char *ser_blockheader_alloc(ssize_t *written, BlockHeader *blockheader);

/**
 * @brief Deserialize a BlockHeader
 *
 * @param dest BlockHeader to write to
 * @param src Buffer of length size_ser_blockheader() to read from
 * @return Number of bytes read if succesfull, -1 otherwise
 */
ssize_t deser_blockheader(BlockHeader *dest, unsigned char *src);

/**
 * @brief Allocate memory and deserialize a BlockHeader
 *
 * @param read Stores number of bytes read if succesfull, -1 otherwise
 * @param src Buffer of length size_ser_blockheader() to read from
 * @return Deserialized blockheader if succesfull, NULL otherwise
 */
BlockHeader *deser_blockheader_alloc(ssize_t *read, unsigned char *src);

/******************************************************************************
 * Blocks
 ******************************************************************************/

/**
 * @brief Get size of a Block if serialized
 *
 * @param block Block to get size of
 * @return Size of block if serialized
 */
size_t size_ser_block(Block *block);

/**
 * @brief Serialize a Block
 *
 * @param dest Buffer of length size_ser_block(block) to write to
 * @param block Block to serialize
 * @return Number of bytes written if succesfull, -1 otherwise
 */
ssize_t ser_block(unsigned char *dest, Block *block);

/**
 * @brief Allocates memory and serializes a Block
 *
 * @param written Stores number of bytes written if succesfull, -1 otherwise
 * @param block Block to serialize
 * @return unsigned char* Serialized block of length size_ser_block(block) if
 *   succesfull, NULL otherwise
 */
unsigned char *ser_block_alloc(ssize_t *written, Block *block);

/**
 * @brief Deserialize a Block
 *
 * @param dest Block to write to
 * @param src Buffer of length size_ser_block(block) to read from
 * @return Number of bytes read if succesfull, -1 otherwise
 */
ssize_t deser_block(Block *dest, unsigned char *src);

/**
 * @brief Allocate memory and deserialize a Block
 *
 * @param read Stores number of bytes read if succesfull, -1 otherwise
 * @param src Buffer of length size_ser_block(block) to read from
 * @return Deserialized block if succesfull, NULL otherwise
 */
Block *deser_block_alloc(ssize_t *read, unsigned char *src);
