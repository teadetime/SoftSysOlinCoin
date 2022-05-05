#pragma once

#include "constants.h"
#include "uthash.h"
#include "utxo_pool.h"
#include "init_db.h"

typedef struct {
  mbedtls_ecdsa_context *key_pair;
  unsigned long amt;
  int spent;  // True when included in a tx but not validated in a block
} WalletEntry;

typedef struct {
  UTXOPoolKey id;
  WalletEntry *entry;
  UT_hash_handle hh;
} WalletPool;

typedef struct {
  unsigned char public_key_hash[PUB_KEY_HASH_LEN];
  mbedtls_ecdsa_context *key_pair;
  UT_hash_handle hh;
} KeyPool;

char *key_pool_path;
leveldb_t *key_pool_db;  // Level DB Database
char *wallet_pool_path;
leveldb_t *wallet_pool_db;  // Level DB Database


/**
 * @brief Initializes wallet

 * @param db_env production or test location to build db
 * @return int 0 if succesful, not zero otherwise
 */
int wallet_init_leveldb(char *db_env);

/**
 * @brief Destroy wallet and keypool db
 * 
 */
void destroy_wallet();

/**
 * @brief Adds an entry to the wallet pool
 *
 * Creates a new WalletEntry using passed data, then adds new entry to the
 * wallet pool. 
 * @param tx Transaction the new entry is a part of
 * @param vout Output index of the new entry within the source transaction
 * @param key_pair Key pair associated with the new entry
 * @return 0 if successful, not zero otherwise
 */
int wallet_pool_build_add_leveldb(Transaction *tx, unsigned int vout, mbedtls_ecdsa_context *key_pair);

/**
 * @brief Wrapper for wallet add to allow for easy updating of wallet pool spent
 * 
 * @param db_key key to update
 * @param entry entry to write
 * @return int 0 if success, not zero otherwise
 */
int wallet_pool_add_wallet_entry_leveldb(unsigned char *db_key, WalletEntry *entry);

/**
 * @brief Find an entry in the wallet pool, freeing is the responsibility of the caller
 * 
 * @param found_entry Null if no entry found, otherwise a malloced wallet entry
 * @param tx_hash Transaction hash corresponding to entry to remove
 * @param vout  Output index corresponding to entry to find
 * @return 0 if found, not zero if not found
 */
int wallet_pool_find_leveldb(WalletEntry **found_entry, unsigned char *tx_hash, unsigned int vout);

/**
 * @brief Remove an entry from the wallet pool
 * 
 * @param tx_hash Transaction hash corresponding to entry to remove
 * @param vout  Output index corresponding to entry to find
 * @return 0 if success, not zero otherwise
 */
int wallet_pool_remove_leveldb(unsigned char *tx_hash, unsigned int vout);

/**
 * @brief Count the number of entries in the wallet pool 
 * 
 * @param num_entries pointer to integer to write the count of the db into
 * @return int 0 if success, not zero otherwise
 */
int wallet_pool_count(unsigned int *num_entries);

/**
 * @brief Frees a wallet entry
 *
 * @param entry Wallet entry to free
 */
void free_wallet_entry(WalletEntry *entry);

/**
 * @brief Add a keypair to the keypool
 * 
 * @param key_pair keypair to add to key pool
 * @return int 0 if success, not zero otherwise
 */
int key_pool_add_leveldb(mbedtls_ecp_keypair *key_pair);

/**
 * @brief Find a keypair in the keypool based on pub_key_hash
 * 
 * @param keypair malloced keypair if found, NULL otherwise
 * @param public_key_hash hash to use as key to locate keypair
 * @return int 0 if success, not zero otherwise
 */
int key_pool_find_leveldb(mbedtls_ecdsa_context **keypair, unsigned char *public_key_hash);

/**
 * @brief Removes a keypair from the keypool
 * 
 * @param public_key_hash hash of key to remove
 * @return int 0 if success, not zero if otherwise
 */
int key_pool_remove_leveldb(unsigned char *public_key_hash);

/**
 * @brief Count number of entries in the keypool
 * 
 * @param num_entries pointer containing resulting count
 * @return int 0 if sucess, not zero if otherwise
 */
int key_pool_count(unsigned int *num_entries);

/**
 * @brief Helper to see if an input to a block is unlockable so that we can 
 * add it to the wallet pool!
 * 
 * @param tx transaction to examine
 * @param vout output number to examine
 * @return mbedtls_ecdsa_context* valid keypair if this output is controllable, 
 * otherwise NULL
 */
mbedtls_ecdsa_context *check_if_output_unlockable_leveldb(Transaction *tx, unsigned int vout);
