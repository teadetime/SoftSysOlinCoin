#include "utxo_pool.h"

void double_spend_set_init(UTXOPool *set);

UTXO *double_spend_add(UTXOPool *set, unsigned char tx_id[TX_HASH_LEN], unsigned int vout);

UTXOPool *double_spend_find_node(UTXOPool *set, unsigned char *tx_hash, unsigned int vout);

UTXOPool *double_spend_find_node_key(UTXOPool *set, UTXOPoolKey *key);

void double_spend_delete(UTXOPool *set);