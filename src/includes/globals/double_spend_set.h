#include "utxo_pool.h"

/**
 * @brief Initialize the set before doing any operations
 * 
 * @param set UTXOPOOl to intialize
 */
void double_spend_set_init(UTXOPool *set);

/**
 * @brief Add an entry to a doublespend set
 * 
 * @param set UTXOPOOL set to add to
 * @param tx_id tx hash corresponding to the utxo
 * @param vout ouput from the tx_id corresponding to the utxo
 * @return int 0 if added successfully, 1 if iem was already in set
 */
int double_spend_add(UTXOPool *set, unsigned char tx_id[TX_HASH_LEN], unsigned int vout);

/**
 * @brief Find UTXO (tx hash and vout) in doublespend set
 * 
 * @param set doublespend set to search
 * @param tx_hash tx hash corresponding to utxo
 * @param vout output number corresponding to utxo
 * @return UTXOPool* NULL if no match, otherwise UTXOPool result
 */
UTXOPool *double_spend_find_node(UTXOPool *set, unsigned char *tx_hash, unsigned int vout);

/**
 * @brief Find UTXO based on UTXOPoolKey
 * 
 * @param set doublespend set to search
 * @param key UTXOPool key to search for
 * @return UTXOPool* NULL if no matching key in hashmap or UTXOPool that matches
 */
UTXOPool *double_spend_find_node_key(UTXOPool *set, UTXOPoolKey *key);

/**
 * @brief Free all elemnts within the datastructure
 * 
 * @param set doublespend UTXOPool to clear
 */
void double_spend_delete(UTXOPool *set);