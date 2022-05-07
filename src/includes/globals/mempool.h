/**
 * @file mempool.h
 * @author nfaber@olin.edu and efisher@olin.edu
 * @brief Memory Pool to keep track of transactions that need validated
 * @version 0.1
 * @date 2022-05-06
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <stdio.h>
#include "constants.h"
#include "uthash.h"
#include "base_tx.h"

/**
 * @brief UTHASH Memory Pool hashmap
 * 
 */
typedef struct MemPool {
  unsigned char id[TX_HASH_LEN];
  Transaction *tx;
  UT_hash_handle hh;
} MemPool;

/**
 * @brief Global MemPool hashmap
 * 
 */
MemPool *mempool;

/**
 * @brief Initializes the global mempool variable
 * 
 */
void mempool_init();

/**
 * @brief Create a new entry in the hashmap with the passed transaction
 * 
 * @param tx Transaction pointer that will be stored in mempool
 * @return Transaction* passed transaction pointer if entry created, NULL otherwise
 */
Transaction *mempool_add(Transaction *tx);

/**
 * @brief Removes the entry corresponding to tx_hash
 * 
 * @param tx_hash Buffer of length TX_HASH_LEN, hash of transaction
 * @return Transaction* transaction pointer stored in removed entry if succesfully
 * removed, NULL otherwise
 */
Transaction *mempool_remove(unsigned char *tx_hash);


/**
 * @brief Finds transaction corresponding to tx_hash
 * 
 * @param tx_hash Buffer of length TX_HASH_LEN, hash of transaction
 * @return Transaction* transaction pointer if found, NULL otherwise
 */
Transaction *mempool_find(unsigned char *tx_hash);

/**
 * @brief Find entry corresponding to tx_hash
 * 
 * @param tx_hash Buffer of length TX_HASH_LEN, hash of transaction
 * @return MemPool* entry if found, NULL otherwise
 */
MemPool *mempool_find_node(unsigned char *tx_hash);

/**
 * @brief Prints a mempool item with the associated id/hash from the hasmap
 * 
 * @param mempool mempool entry to print
 * @param prefix tring to put in front of all print commands used for tabbing structure
 */
void print_mempool(MemPool *mempool, char *prefix);

/**
 * @brief Prints all transactions in mempool hashmap to stdout
 * 
 * @param prefix string to put in front of all print commands used for tabbing structure
 */
void print_mempool_hashmap(char *prefix);
