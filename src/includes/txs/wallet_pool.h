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
 * output with with a public key hash that corresponds to a keypair in the key
 * pool.
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

void wallet_init();

WalletEntry *wallet_pool_add(Transaction *tx, unsigned int vout, mbedtls_ecdsa_context *key_pair);
WalletEntry *wallet_pool_remove(unsigned char *tx_hash, unsigned int vout);
WalletEntry *wallet_pool_remove_key(UTXOPoolKey *key);
WalletEntry *wallet_pool_find(unsigned char *tx_hash, unsigned int vout);
WalletPool *wallet_pool_find_node(unsigned char *tx_hash, unsigned int vout);
WalletPool *wallet_pool_find_node_key(UTXOPoolKey *key);

mbedtls_ecp_keypair *key_pool_add(mbedtls_ecp_keypair *key_pair);
mbedtls_ecp_keypair *key_pool_remove(unsigned char *public_key_hash);
mbedtls_ecp_keypair *key_pool_find(unsigned char *public_key_hash);
KeyPool *key_pool_find_node(unsigned char *public_key_hash);
