#pragma once

#include "constants.h"
#include "uthash.h"
#include "base_tx.h"
#include "leveldb/c.h"
#define UTXO_POOL_KEY_LEN TX_HASH_LEN+sizeof(((Transaction*)0)->inputs->prev_utxo_output)

typedef struct UTXOPoolKey {
  unsigned char tx_hash[TX_HASH_LEN];
  unsigned int vout;
} UTXOPoolKey;

typedef struct UTXOPool {
  UTXOPoolKey id;
  UTXO *utxo;
  UT_hash_handle hh;
} UTXOPool;

char *utxo_pool_path;
leveldb_t *utxo_pool_db;  // Level DB Database

/**
 * @brief Initializes Global Database, database must not be open by another process
 * 
 * @return int 0 if successful
 */
int utxo_pool_init_leveldb(char *db_env);

/**
 * @brief Creates key for utxopool 
 * 
 * @param dest pointer to character buffer to write into
 * @param len pointer to integer containing length of alloced dest
 * @param hash hash of transaction for the utxo
 * @param vout output to use
 * @return int 0 if success not zero if failure
 */
int make_utxo_pool_key_with_hash(unsigned char *dest, size_t *len, unsigned char *hash, unsigned int vout);

/**
 * @brief Make Utxo pool from a transaction and vout
 * 
 * @param dest pointer to character buffer to write into
 * @param len pointer to integer containing length of alloced dest
 * @param tx transaction to use as key for utxo
 * @param vout output to use as utxo
 * @return int 0 if success not zero if failure
 */
int make_utxo_pool_key(unsigned char *dest, size_t *len, Transaction *tx, unsigned int vout);

/**
 * @brief Add a transaction to the utxo Pool
 * 
 * @param tx Transaction to add
 * @param vout vout for the utxo
 * @return int 0 if success not zero if failure
 */
int utxo_pool_add_leveldb(Transaction *tx, unsigned int vout);

/**
 * @brief Find a UTXO in the utxopool
 * 
 * @param found_utxo pointer to utxo pointer to store found result in freeing is responsibility of the caller
 * @param tx_hash hash for utxo to find
 * @param vout output for utxo to find
 * @return int 0 if success not zero if failure
 */
int utxo_pool_find_leveldb(UTXO **found_utxo, unsigned char *tx_hash, unsigned int vout);

/**
 * @brief Remove utxo from utxopool
 * 
 * @param tx_hash hash to remove from the utxopool
 * @param vout output to remove from the mempool
 * @return int 0 if success not zero if failure
 */
int utxo_pool_remove_leveldb(unsigned char *tx_hash, unsigned int vout);

/**
 * @brief Count number of items in utxo_pool
 * 
 * @param num_entries pointer to hold returned number of values in the utxopool
 * @return int 0 if success not zero if failure
 */
int utxo_pool_count(unsigned int *num_entries);

/**
 * @brief Prints a utxo to stdout, for visualization
 * 
 * @param utxo utxo to print
 * @param prefix tring to put in front of all print commands used for tabbing structure
 */
void print_utxo(UTXO *utxo, char *prefix);

/*
Prints all of the utxo_pool hashmap to stdout with keys for visualizatoin

prefix: string to put in front of all print commands used for tabbing structure
*/
void print_utxo_hashmap(char *prefix);
