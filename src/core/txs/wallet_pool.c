#include <stdio.h>
#include "wallet_pool.h"
#include "crypto.h"
#include "utxo_pool.h"
#include "ser_wallet.h"

int wallet_init_leveldb(char *db_env){
  int init_wallet_pool = init_db(&wallet_pool_db, &wallet_pool_path, db_env, "/wallet_pool");
  int init_key_pool = init_db(&key_pool_db, &key_pool_path, db_env, "/key_pool");
  if(init_wallet_pool != 0){
    return 5;
  }
  if(init_key_pool != 0){
    return 6;
  }
  return 0;
}

void destroy_wallet(){
  destroy_db(&wallet_pool_db, wallet_pool_path);
  destroy_db(&key_pool_db, key_pool_path);
}

/* WALLET POOL FUNCS */
int wallet_pool_add_wallet_entry_leveldb(unsigned char *db_key, WalletEntry *entry){
  char *err = NULL;
  size_t wallet_entry_size;
  unsigned char *serialized_wallet_entry = ser_wallet_entry_alloc(&wallet_entry_size, entry);
  if(!serialized_wallet_entry ){
    return 3;
  }
  leveldb_writeoptions_t *woptions = leveldb_writeoptions_create();
  leveldb_put(wallet_pool_db, woptions, db_key, UTXO_POOL_KEY_LEN, serialized_wallet_entry, wallet_entry_size, &err);
  leveldb_writeoptions_destroy(woptions);
  free(serialized_wallet_entry);

  if (err != NULL) {
    fprintf(stderr, "Write fail: %s\n", err);
    leveldb_free(err);
    return 1;
  }
  return 0;
}

int wallet_pool_build_add_leveldb(Transaction *tx, unsigned int vout, mbedtls_ecdsa_context *key_pair){
  unsigned char db_key[UTXO_POOL_KEY_LEN];
  size_t key_len;
  //Make Key
  if(make_utxo_pool_key(db_key, &key_len, tx, vout) != 0){
    return 2;
  }
  // Build content
  WalletEntry *content = malloc(sizeof(WalletEntry));
  content->amt = tx->outputs[vout].amt;
  content->key_pair = key_pair;
  content->spent = 0;
  int ret_add = wallet_pool_add_wallet_entry_leveldb(db_key, content);
  free(content);
  return ret_add;
}

int wallet_pool_find_leveldb(WalletEntry **found_entry, unsigned char *tx_hash, unsigned int vout) {
  unsigned char db_key[UTXO_POOL_KEY_LEN];
  size_t key_len;
  char *err = NULL;
  size_t read_len;
  char *read = NULL;
  //Make Key
  if(make_utxo_pool_key_with_hash(db_key, &key_len, tx_hash, vout) != 0){
    return 2;
  }

  leveldb_readoptions_t *roptions = leveldb_readoptions_create();
  read = leveldb_get(wallet_pool_db, roptions, db_key, key_len, &read_len, &err);
  leveldb_readoptions_destroy(roptions);

  if (err != NULL) {
    fprintf(stderr, "Read fail: %s\n", err);
    leveldb_free(err);
    if(read != NULL){
      free(read);
    }
    return 3;
  }
  if(read == NULL){
    return 1;
  }
  size_t read_bytes;
  *found_entry = deser_wallet_entry_alloc(&read_bytes, (unsigned char*) read);
  free(read);
  if(!*found_entry){
    return 4;
  }
  return 0;
}

int wallet_pool_remove_leveldb(unsigned char *tx_hash, unsigned int vout){
  unsigned char db_key[UTXO_POOL_KEY_LEN];
  size_t key_len;
  char *err = NULL;
  //Make Key
  if(make_utxo_pool_key_with_hash(db_key, &key_len, tx_hash, vout) != 0){
    return 2;
  }
  leveldb_writeoptions_t *woptions = leveldb_writeoptions_create();
  leveldb_delete(wallet_pool_db, woptions, db_key, key_len, &err);
  leveldb_writeoptions_destroy(woptions);
  if (err != NULL) {
    fprintf(stderr, "Delete fail: %s\n", err);
    leveldb_free(err);
    return(3);
  }
  return 0;
}

int wallet_pool_count(unsigned int *num_entries){
  return db_count(wallet_pool_db, num_entries);
}

void free_wallet_entry(WalletEntry *entry) {
  if (entry == NULL)
    return;
  if (entry->key_pair != NULL)
    mbedtls_ecp_keypair_free(entry->key_pair);
  free(entry);
}

/* KEY POOL FUNCS */
int key_pool_add_leveldb(mbedtls_ecp_keypair *key_pair) {
  unsigned char db_key[PUB_KEY_HASH_LEN];
  hash_pub_key(db_key, key_pair);
  char *err = NULL;

  size_t key_pair_entry_size;
  unsigned char *serialized_key_pool_entry = ser_keypair_alloc(&key_pair_entry_size, key_pair);
  if(!serialized_key_pool_entry){
    return 3;
  }
  leveldb_writeoptions_t *woptions = leveldb_writeoptions_create();
  leveldb_put(key_pool_db, woptions, db_key, PUB_KEY_HASH_LEN, serialized_key_pool_entry, key_pair_entry_size, &err);
  leveldb_writeoptions_destroy(woptions);
  free(serialized_key_pool_entry);

  if (err != NULL) {
    fprintf(stderr, "Write fail: %s\n", err);
    leveldb_free(err);
    return 1;
  }
  return 0;
}

int key_pool_find_leveldb(mbedtls_ecdsa_context **keypair, unsigned char *public_key_hash) {
  char *err = NULL;
  size_t read_len;
  char *read = NULL;

  leveldb_readoptions_t *roptions = leveldb_readoptions_create();
  read = leveldb_get(key_pool_db, roptions, public_key_hash, PUB_KEY_HASH_LEN, &read_len, &err);
  leveldb_readoptions_destroy(roptions);

  if (err != NULL) {
    fprintf(stderr, "Read fail: %s\n", err);
    leveldb_free(err);
    if(read != NULL){
      free(read);
    }
    return 3;
  }
  if(read == NULL){
    return 1;
  }

  size_t read_bytes;
  *keypair = deser_keypair_alloc(&read_bytes, (unsigned char*) read);
  free(read);
  if(!*keypair){
    return 4;
  }
  return 0;
}

int key_pool_remove_leveldb(unsigned char *public_key_hash) {
  char *err = NULL;
  leveldb_writeoptions_t *woptions = leveldb_writeoptions_create();
  leveldb_delete(key_pool_db, woptions, public_key_hash, PUB_KEY_HASH_LEN, &err);
  leveldb_writeoptions_destroy(woptions);
  if (err != NULL) {
    fprintf(stderr, "Delete fail: %s\n", err);
    leveldb_free(err);
    return(3);
  }
  return 0;
}

int key_pool_count(unsigned int *num_entries){
  return db_count(key_pool_db, num_entries);
}

mbedtls_ecdsa_context *check_if_output_unlockable_leveldb(Transaction *tx, unsigned int vout){
  mbedtls_ecdsa_context *ret_keypair = NULL;
  int ret_find = key_pool_find_leveldb(&ret_keypair, tx->outputs[vout].public_key_hash);
  if(ret_find != 0){
    return NULL;
  }
  return ret_keypair;
}
