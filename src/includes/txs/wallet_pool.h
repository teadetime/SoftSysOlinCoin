#pragma once

#include "constants.h"
#include "uthash.h"
#include "utxo_pool.h"

typedef struct {
  mbedtls_ecdsa_context *key_pair;
  unsigned long amt;
  int spent;
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

WalletPool *wallet_pool;
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
