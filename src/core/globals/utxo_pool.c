#include <stdio.h>
#include "utxo_pool.h"
#include "crypto.h"
#include "init_db.h"
#include "ser_tx.h"

int make_utxo_pool_key_with_hash(unsigned char *dest, size_t *len, unsigned char *hash, unsigned int vout){
  *len = TX_HASH_LEN+sizeof(vout);
  memcpy(dest, hash, TX_HASH_LEN);
  memcpy(dest+TX_HASH_LEN, &vout, sizeof(vout));
  return 0;
}

int make_utxo_pool_key(unsigned char *dest, size_t *len, Transaction *tx, unsigned int vout){
  if(vout > tx->num_outputs-1){
    return 1;
  }
  unsigned char temp_hash[TX_HASH_LEN];
  hash_tx(temp_hash, tx);
  return make_utxo_pool_key_with_hash(dest, len, temp_hash, vout);
}

int utxo_pool_init_leveldb(char *db_env){
  return init_db(&utxo_pool_db, &utxo_pool_path, db_env, "/utxo_pool");
}

int utxo_pool_add_leveldb(Transaction *tx, unsigned int vout){
  unsigned char db_key[UTXO_POOL_KEY_LEN];
  size_t key_len;
  char *err = NULL;
  //Make Key
  if(make_utxo_pool_key(db_key, &key_len, tx, vout) != 0){
    return 2;
  }
  // Make Value (Build new UTXO)
  UTXO utxo;
  utxo.amt = tx->outputs[vout].amt;
  memcpy(utxo.public_key_hash, tx->outputs[vout].public_key_hash, PUB_KEY_HASH_LEN);
  
  size_t utxo_size;
  unsigned char *serialized_utxo = ser_utxo_alloc(&utxo_size, &utxo);
  if(!serialized_utxo){
    return 3;
  }
  leveldb_writeoptions_t *woptions = leveldb_writeoptions_create();
  leveldb_put(utxo_pool_db, woptions, db_key, key_len, serialized_utxo, utxo_size, &err);
  leveldb_writeoptions_destroy(woptions);
  free(serialized_utxo);
  dump_buf("", "KEY: ", db_key, key_len);

  if (err != NULL) {
    fprintf(stderr, "Write fail: %s\n", err);
    leveldb_free(err);
    return(1);
  }

  return 0;
}

int utxo_pool_find_leveldb(UTXO **found_utxo, unsigned char *tx_hash, unsigned int vout){
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
  read = leveldb_get(utxo_pool_db, roptions, db_key, key_len, &read_len, &err);
  leveldb_readoptions_destroy(roptions);

  if (err != NULL) {
    fprintf(stderr, "Read fail: %s\n", err);
    leveldb_free(err);
    return 3;
  }
  if(read == NULL){
    return 1;
  }

  *found_utxo = deser_utxo_alloc(NULL, (unsigned char*) read);
  free(read);
  if(!*found_utxo){
    return 4;
  }
  return 0;
}

int utxo_pool_remove_leveldb(unsigned char *tx_hash, unsigned int vout){
  unsigned char db_key[UTXO_POOL_KEY_LEN];
  size_t key_len;
  char *err = NULL;
  //Make Key
  if(make_utxo_pool_key_with_hash(db_key, &key_len, tx_hash, vout) != 0){
    return 2;
  }
  leveldb_writeoptions_t *woptions = leveldb_writeoptions_create();
  leveldb_delete(utxo_pool_db, woptions, db_key, key_len, &err);
  leveldb_writeoptions_destroy(woptions);
  if (err != NULL) {
    fprintf(stderr, "Delete fail: %s\n", err);
    leveldb_free(err);
    return(3);
  }
  return 0;
}

int utxo_pool_count(unsigned int *num_entries){
  return db_count(utxo_pool_db, utxo_pool_path, num_entries);
}

void print_utxo(UTXO *utxo, char *prefix){
  char *sub_prefix = malloc(strlen(prefix)+strlen(PRINT_TAB)+1);
  strcpy(sub_prefix, prefix);
  strcat(sub_prefix, PRINT_TAB);
  printf("%sUTXO Sizeof(%li):\n", prefix, sizeof(*utxo));
  printf("%samount: %li\n", sub_prefix, utxo->amt);
  dump_buf(sub_prefix, "public_key_hash:", utxo->public_key_hash, PUB_KEY_HASH_LEN);
  free(sub_prefix);
}

void print_utxo_hashmap(char *prefix){
  char *sub_prefix = malloc(strlen(prefix)+strlen(PRINT_TAB)+1);
  strcpy(sub_prefix, prefix);
  strcat(sub_prefix, PRINT_TAB);

  unsigned int num_items;
  utxo_pool_count(&num_items);
  printf("%sUTXO Hashmap items(%i):\n", prefix, num_items);

  char *err = NULL;
  leveldb_readoptions_t *roptions;
  roptions = leveldb_readoptions_create();
  leveldb_iterator_t *iter = leveldb_create_iterator(utxo_pool_db, roptions);

  for (leveldb_iter_seek_to_first(iter); leveldb_iter_valid(iter); leveldb_iter_next(iter))
  {
      size_t key_len, value_len;
      unsigned const char *key_ptr = (unsigned const char*) leveldb_iter_key(iter, &key_len);
      unsigned const char *value_ptr = (unsigned const char*) leveldb_iter_value(iter, &value_len);

      printf("%shashmap_id:\n", prefix);
      dump_buf(sub_prefix, "tx_hash:", key_ptr, TX_HASH_LEN);
      printf("%svout: %d\n", sub_prefix, *(unsigned int *) (key_ptr+TX_HASH_LEN));
      

      UTXO *read_utxo = deser_utxo_alloc(NULL, (unsigned char*)value_ptr);
      print_utxo(read_utxo, prefix);
      // /* Prints some binary noise with the data */
  }
  leveldb_iter_destroy(iter);
  leveldb_readoptions_destroy(roptions);
  leveldb_free(err); 
  free(sub_prefix);
}