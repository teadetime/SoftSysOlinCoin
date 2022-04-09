#include <stdio.h>
#include "wallet_pool.h"
#include "crypto.h"

void wallet_init() {
  wallet_pool = NULL;
  key_pool = NULL;
}

/* WALLET POOL FUNCS */

WalletEntry *wallet_pool_add(
    Transaction *tx, unsigned int vout, mbedtls_ecdsa_context *key_pair
) {
  WalletPool *new_entry, *found_entry;
  WalletEntry *content;

  new_entry = malloc(sizeof(WalletPool));

  // Build key
  memset(&(new_entry->id), 0, sizeof(UTXOPoolKey));
  hash_tx(new_entry->id.tx_hash, tx);
  new_entry->id.vout = vout;

  // Build content
  content = malloc(sizeof(WalletEntry));
  content->amt = tx->outputs[vout].amt;
  content->key_pair = key_pair;
  content->spent = 0;

  new_entry->entry = content;

  found_entry = wallet_pool_find_node_key(&(new_entry->id));
  if (found_entry == NULL) {
    HASH_ADD(hh, wallet_pool, id, sizeof(UTXOPoolKey), new_entry);
    return content;
  } else {
    free(new_entry->entry);
    free(new_entry);
    return NULL;
  }
}

void wallet_pool_remove(unsigned char *tx_hash, unsigned int vout) {
  WalletPool *entry;

  entry = wallet_pool_find_node(tx_hash, vout);
  if (entry != NULL) {
    HASH_DEL(wallet_pool, entry);
    free(entry->entry);
    free(entry);
  }
}

WalletEntry *wallet_pool_find(unsigned char *tx_hash, unsigned int vout) {
  WalletPool *found_entry;

  found_entry = wallet_pool_find_node(tx_hash, vout);
  if (found_entry != NULL) {
    return found_entry->entry;
  }

  return NULL;
}

WalletPool *wallet_pool_find_node(unsigned char *tx_hash, unsigned int vout) {
  UTXOPoolKey key;

  memset(&key, 0, sizeof(UTXOPoolKey));
  memcpy(key.tx_hash, tx_hash, TX_HASH_LEN);
  key.vout = vout;

  return wallet_pool_find_node_key(&key);
}

WalletPool *wallet_pool_find_node_key(UTXOPoolKey *key) {
  WalletPool *found_entry;
  HASH_FIND(hh, wallet_pool, key, sizeof(UTXOPoolKey), found_entry);
  return found_entry;
}

/* KEY POOL FUNCS */

mbedtls_ecdsa_context *key_pool_add(mbedtls_ecp_keypair *key_pair) {
  KeyPool *new_entry, *found_entry;

  new_entry = malloc(sizeof(KeyPool));
  hash_pub_key(new_entry->public_key_hash, key_pair);
  new_entry->key_pair = key_pair;

  found_entry = key_pool_find_node(new_entry->public_key_hash);
  if (found_entry == NULL) {
    HASH_ADD(hh, key_pool, public_key_hash, PUB_KEY_HASH_LEN, new_entry);
    return key_pair;
  } else {
    free(new_entry);
    return NULL;
  }
}

mbedtls_ecdsa_context *key_pool_remove(unsigned char *public_key_hash) {
  KeyPool *entry;
  mbedtls_ecdsa_context *key_pair;

  entry = key_pool_find_node(public_key_hash);
  if (entry != NULL) {
    HASH_DEL(key_pool, entry);
    key_pair = entry->key_pair;
    free(entry);
    return key_pair;
  }

  return NULL;
}

mbedtls_ecdsa_context *key_pool_find(unsigned char *public_key_hash) {
  KeyPool *found_entry;

  found_entry = key_pool_find_node(public_key_hash);
  if (found_entry != NULL) {
    return found_entry->key_pair;
  }

  return NULL;
}

KeyPool *key_pool_find_node(unsigned char *public_key_hash) {
  KeyPool *found_entry;
  HASH_FIND(hh, key_pool, public_key_hash, PUB_KEY_HASH_LEN, found_entry);
  return found_entry;
}

mbedtls_ecdsa_context *check_if_output_unlockable(Transaction *tx, unsigned int vout){
  return key_pool_find(tx->outputs[vout].public_key_hash);
}
