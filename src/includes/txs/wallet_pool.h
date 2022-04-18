#pragma once

#include "constants.h"
#include "uthash.h"
#include "utxo_pool.h"

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

/* The wallet pool tracks all outputs the wallet controls
 *
 * Entries should be added whenever the wallet receives a new block and finds an
 * output with a public key hash that corresponds to a keypair in the key pool.
 *
 * Entires should be removed whenever the wallet receives a new block and finds
 * an input with an UTXOPoolKey (tx hash + vout) that corresponds to an existing
 * entry.
 */
WalletPool *wallet_pool;

/* The key pool tracks all keys the wallet controls
 *
 * Entries should be added whenever a new key is generated - whether for use in
 * a transaction with an output back to the wallet, or for use as an address to
 * give to another member of the network.
 *
 * Entries in the key pool should almost NEVER be removed. Old keys can always
 * be reused, and outputs to a key pair that we no longer control are un-usable
 */
KeyPool *key_pool;

/**
 * @brief Initializes wallet
 *
 * Initializes the wallet pool and key pool
 */
void wallet_init();

/* WALLET POOL FUNCTIONS */

/**
 * @brief Adds an entry to the wallet pool
 *
 * Creates a new WalletEntry using passed data, then adds new entry to the
 * wallet pool. NOTE: Does not copy the passed key - only stores the pointer. We
 * expect keys to be never freed.
 *
 * @param tx Transaction the new entry is a part of
 * @param vout Output index of the new entry within the source transaction
 * @param key_pair Key pair associated with the new entry
 * @return New entry if succesfull, otherwise NULL
 */
WalletEntry *wallet_pool_add(Transaction *tx, unsigned int vout, mbedtls_ecdsa_context *key_pair);

/**
 * @brief Removes an entry from the wallet pool
 *
 * @param tx_hash Transaction hash corresponding to entry to find
 * @param vout Output index corresponding to entry to find
 */
void wallet_pool_remove(unsigned char *tx_hash, unsigned int vout);

/**
 * @brief Finds an entry in the wallet pool
 *
 * @param tx_hash Transaction hash corresponding to entry to remove
 * @param vout  Output index corresponding to entry to find
 * @return Entry if found, otherwise NULL
 */
WalletEntry *wallet_pool_find(unsigned char *tx_hash, unsigned int vout);

/**
 * @brief Finds a node in the wallet pool
 *
 * Internal function that returns the WalletPool wrapper type instead of the
 * WalletEntry type.
 *
 * @param tx_hash Transaction hash corresponding to entry to remove
 * @param vout  Output index corresponding to entry to find
 * @return Node if found, otherweise NULL
 */
WalletPool *wallet_pool_find_node(unsigned char *tx_hash, unsigned int vout);

/**
 * @brief Finds a node in the wallet pool
 *
 * Internal function that takes in a UTXOPoolKey and returns the WalletPool
 * wrapper type instead of the WalletEntry type.
 *
 * @param key UTXOPoolKey containing tx_hash and vout
 * @return Node if found, otherweise NULL
 */
WalletPool *wallet_pool_find_node_key(UTXOPoolKey *key);

/* KEY POOL FUNCTIONS */

/**
 * @brief Adds a key pair to the key pool
 *
 * Stores the key pair in the key pool with the hash of the public key as the
 * dictionary key
 *
 * @param key_pair Key pair to store
 * @return Passed key pool if succesfull, NULL otherwise
 */
mbedtls_ecp_keypair *key_pool_add(mbedtls_ecp_keypair *key_pair);

/**
 * @brief Removes a key pair from the key pool
 *
 * @param public_key_hash Public key hash corresponding to key pair to remove
 * @return Key pair if removed, otherwise NULL
 */
mbedtls_ecp_keypair *key_pool_remove(unsigned char *public_key_hash);

/**
 * @brief Finds a key pair in the key pool
 *
 * @param public_key_hash Public key hash corresponding to key pair to remove
 * @return Key pair if found, otherwise NULL
 */
mbedtls_ecp_keypair *key_pool_find(unsigned char *public_key_hash);

/**
 * @brief Finds a node in the key pool
 *
 * Internal function that returns the KeyPool wrapper type instead of a key pair
 *
 * @param public_key_hash Public key hash corresponding to key pair to remove
 * @return Node if found, otherwise NULL
 */
KeyPool *key_pool_find_node(unsigned char *public_key_hash);

/**
 * @brief Helper to see if an input to a block is unlockable so that we can 
 * add it to the wallet pool!
 * 
 * @param tx transaction to examine
 * @param vout output number to examine
 * @return mbedtls_ecdsa_context* valid keypair if this output is controllable, 
 * otherwise NULL
 */
mbedtls_ecdsa_context *check_if_output_unlockable(Transaction *tx, unsigned int vout);
