#pragma once

#include "utxo_pool.h"

typedef struct {
  UTXOPoolKey id;
  unsigned char tx_hash[TX_HASH_LEN];
  UT_hash_handle hh;
} UTXOToTx;

UTXOToTx *utxo_to_tx;

/* Initializes the global utxo_pool variable */
void utxo_to_tx_init();

/* Creates a new UTXO using transaction and output number, then ceates a new
 * entry in the hashmap
 * 
 * Returns pointer to newly created UTXO if entry created, NULL otherwise
 *
 * transaction: Transaction pointer to transaction the new UTXO is an output of
 * vout: Which output of the passed transaction the new UTXO is
 */
int utxo_to_tx_add(unsigned char *utxo_tx_hash, unsigned int vout, unsigned char *tx_hash);

int utxo_to_tx_add_tx(Transaction *tx);

/* Removes the entry corresponding to transaction hash and output number
 *
 * Returns the UTXO pointer stored in removed entry if succesfully removed, NULL
 * otherwise
 *
 * tx_hash: Buffer of length TX_HASH_LEN, hash of transaction the UTXO is from
 * vout: Which output of the transaction the UTXO is from
 */
int utxo_to_tx_remove(unsigned char *utxo_tx_hash, unsigned int vout);

/* Finds UTXO corresponding to transaction hash and output number
 *
 * Returns UTXO pointer if found, NULL otherwise
 *
 * tx_hash: Buffer of length TX_HASH_LEN, hash of transaction the UTXO is from
 * vout: Which output of the transaction the UTXO is from
 */
int utxo_to_tx_find(unsigned char *dest, unsigned char *tx_hash, unsigned int vout);

/* Finds entry corresponding to transaction hash and output number
 *
 * Returns entry if found, NULL otherwise
 *
 * tx_hash: Buffer of length TX_HASH_LEN, hash of transaction the UTXO is from
 * vout: Which output of the transaction the UTXO is from
 */
UTXOToTx *utxo_to_tx_find_node(unsigned char *tx_hash, unsigned int vout);

/* Finds entry corresponding to UTXOPoolKey. Same behavior as
 * utxo_to_tx_find_node, but takes UTXOPoolKey instead. Primarily an internal
 * func
 *
 * Returns entry if found, NULL otherwise
 *
 * key: UTXOPoolKey struct containing tx_hash and vout to query for
 */
UTXOToTx *utxo_to_tx_find_node_key(UTXOPoolKey *key);
