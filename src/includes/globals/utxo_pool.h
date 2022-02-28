#pragma once

#include "constants.h"
#include "uthash.h"
#include "base_tx.h"

typedef struct UTXOPoolKey {
  unsigned char tx_hash[TX_HASH_LEN];
  unsigned int vout;
} UTXOPoolKey;

typedef struct UTXOPool {
  UTXOPoolKey id;
  UTXO *utxo;
  UT_hash_handle hh;
} UTXOPool;

UTXOPool *utxo_pool;

/* Initializes the global utxo_pool variable */
void utxo_pool_init();

/* Creates a new UTXO using transaction and output number, then ceates a new
 * entry in the hashmap
 * 
 * Returns pointer to newly created UTXO if entry created, NULL otherwise
 *
 * transaction: Transaction pointer to transaction the new UTXO is an output of
 * vout: Which output of the passed transaction the new UTXO is
 */
UTXO *utxo_pool_add(Transaction *tx, unsigned int vout);

/* Removes the entry corresponding to transaction hash and output number
 *
 * Returns the UTXO pointer stored in removed entry if succesfully removed, NULL
 * otherwise
 *
 * tx_hash: Buffer of length TX_HASH_LEN, hash of transaction the UTXO is from
 * vout: Which output of the transaction the UTXO is from
 */
UTXO *utxo_pool_remove(unsigned char *tx_hash, unsigned int vout);

/* Removes the entry corresponding to UTXOPoolKey. Same behavior as
 * utxo_pool_remove, but takes UTXOPoolKey instead. Primary an internal func
 *
 * Returns the UTXO pointer stored in removed entry if succesfully removed, NULL
 * otherwise
 *
 * key: UTXOPoolKey struct containing tx_hash and vout to query for
 */
UTXO *utxo_pool_remove_key(UTXOPoolKey *key);

/* Finds UTXO corresponding to transaction hash and output number
 *
 * Returns UTXO pointer if found, NULL otherwise
 *
 * tx_hash: Buffer of length TX_HASH_LEN, hash of transaction the UTXO is from
 * vout: Which output of the transaction the UTXO is from
 */
UTXO *utxo_pool_find(unsigned char *tx_hash, unsigned int vout);

/* Finds entry corresponding to transaction hash and output number
 *
 * Returns entry if found, NULL otherwise
 *
 * tx_hash: Buffer of length TX_HASH_LEN, hash of transaction the UTXO is from
 * vout: Which output of the transaction the UTXO is from
 */
UTXOPool *utxo_pool_find_node(unsigned char *tx_hash, unsigned int vout);

/* Finds entry corresponding to UTXOPoolKey. Same behavior as
 * utxo_pool_find_node, but takes UTXOPoolKey instead. Primarily an internal
 * func
 *
 * Returns entry if found, NULL otherwise
 *
 * key: UTXOPoolKey struct containing tx_hash and vout to query for
 */
UTXOPool *utxo_pool_find_node_key(UTXOPoolKey *key);
