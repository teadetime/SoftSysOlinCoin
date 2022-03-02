#pragma once

#include "constants.h"
#include "uthash.h"
#include "base_tx.h"

typedef struct MemPool {
  unsigned char id[TX_HASH_LEN];
  Transaction *tx;
  UT_hash_handle hh;
} MemPool;

MemPool *mempool;

/* Initializes the global mempool variable */
void mempool_init();

/* Creates a new entry in the hashmap with the passed transaction
 *
 * Returns passed transaction pointer if entry created, NULL otherwise
 *
 * transaction: Transaction pointer that will be stored in entry
 */
Transaction *mempool_add(Transaction *tx);

/* Removes the entry corresponding to tx_hash
 *
 * Returns the transaction pointer stored in removed entry if succesfully
 * removed, NULL otherwise
 *
 * tx_hash: Buffer of length TX_HASH_LEN, hash of transaction
 */
Transaction *mempool_remove(unsigned char *tx_hash);

/* Finds transaction corresponding to tx_hash
 *
 * Returns transaction pointer if found, NULL otherwise
 *
 * tx_hash: Buffer of length TX_HASH_LEN, hash of transaction
 */
Transaction *mempool_find(unsigned char *tx_hash);

/* Finds entry corresponding to tx_hash
 *
 * Returns entry if found, NULL otherwise
 *
 * tx_hash: Buffer of length TX_HASH_LEN, hash of transaction
 */
MemPool *mempool_find_node(unsigned char *tx_hash);
