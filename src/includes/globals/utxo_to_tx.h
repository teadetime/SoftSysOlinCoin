#pragma once

#include "utxo_pool.h"

typedef struct {
  UTXOPoolKey id;
  unsigned char tx_hash[TX_HASH_LEN];
  UT_hash_handle hh;
} UTXOToTx;

UTXOToTx *utxo_to_tx;

/**
 * @brief Initializes the global utxo_to_tx variable
 */
void utxo_to_tx_init();

/**
 * @brief Adds a UTXO ID + transaction key value pair to the hash map
 *
 * @param utxo_tx_hash Transaction hash for UTXO, has length TX_HASH_LEN
 * @param vout Vout for UTXO
 * @param tx_hash Transaction hash of transaction to map to
 * @return 0 if add successful, 1 if not
 */
int utxo_to_tx_add(unsigned char *utxo_tx_hash, unsigned int vout, unsigned char *tx_hash);

/**
 * @brief Adds UTXO / Transaction key value pairs based on a transactions inputs
 *
 * NOTE: Throws an error and exits if transaction contains an input which
 * references a UTXO that is already in the mapping. Ensure transaction is
 * validated against mempool before calling this function to avoid.
 * 
 * @param tx Transaction to update mapping with
 * @return 0 if updat was successful, 0 otherwise
 */
int utxo_to_tx_add_tx(Transaction *tx);

/**
 * @brief Removes entry correspondinig to utxo transaction hash and vout
 *
 * @param utxo_tx_hash Transaction hash for utxo key
 * @param vout Vout for utxo key
 * @return 0 if entry removed, 1 if entry not found
 */
int utxo_to_tx_remove(unsigned char *utxo_tx_hash, unsigned int vout);

/**
 * @brief Finds transaction hash corresponding to utxo id
 *
 * @param dest Buffer to write to, has length TX_HASH_LEN
 * @param utxo_tx_hash Transaction hash for utxo key
 * @param vout Vout for utxo key
 * @return 0 if entry found, 1 otherwise
 */
int utxo_to_tx_find(unsigned char *dest, unsigned char *utxo_tx_hash, unsigned int vout);

/**
 * @brief Finds entry corresponding to utxo id
 *
 * Primarily used internally when an entry is needed
 *
 * @param utxo_tx_hash Transaction hash for utxo key
 * @param vout Vout for utxo key
 * @return Pointer to entry if found, NULL otherwise
 */
UTXOToTx *utxo_to_tx_find_node(unsigned char *utxo_tx_hash, unsigned int vout);

/**
 * @brief Finds entry corresponding to UTXOPoolKey struct
 * 
 * Primarily used internally when an entry is needed and a UTXOPoolKey has
 * already been constructed
 *
 * @param key UTXO id values
 * @return Pointer to entry if found, NULL otherwise
 */
UTXOToTx *utxo_to_tx_find_node_key(UTXOPoolKey *key);
