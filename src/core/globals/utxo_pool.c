#include <stdio.h>
#include "utxo_pool.h"
#include "crypto.h"
#include "init_db.h"
#include "ser_tx.h"

int make_utxo_pool_key(unsigned char *dest[], size_t *len, Transaction *tx, unsigned int vout){
  if(vout > tx->num_outputs-1){
    return 1;
  }
  *len = TX_HASH_LEN+sizeof(vout);
  *dest = malloc(*len);
  hash_tx(*dest, tx);
  memcpy(*dest+TX_HASH_LEN, &vout, sizeof(vout));
  return 0;
}

void utxo_pool_init_leveldb(){
  set_db_path(&utxo_pool_path, "/utxo_pool");
  open_or_create_db(&utxo_pool_db, utxo_pool_path);

  //Maybe close the DB here?
}

void utxo_pool_init() {
  utxo_pool = NULL;
}

int utxo_pool_add_leveldb(Transaction *tx, unsigned int vout){
  if(check_if_db_loaded(&utxo_pool_db, utxo_pool_path) != 0){
    return 5;
  }
  // NOTE WE SHOULD CHECK TO SEE IF VOUT IS LARGER THAN NUM OUTPUTS!
  unsigned char *db_key;
  size_t key_len;
  char *err = NULL;
  //Make Key
  if(make_utxo_pool_key(&db_key, &key_len, tx, vout) != 0){
    return 2;
  }
  // Make Value (Build new UTXO)
  UTXO *utxo = malloc(sizeof(UTXO));
  utxo->amt = tx->outputs[vout].amt;
  memcpy(utxo->public_key_hash, tx->outputs[vout].public_key_hash, PUB_KEY_HASH_LEN);
  unsigned char *serialized_utxo = ser_utxo(utxo);
  size_t utxo_size = sizeof(UTXO);

  
  leveldb_writeoptions_t *woptions = leveldb_writeoptions_create();
  leveldb_put(utxo_pool_db, woptions, db_key, key_len, serialized_utxo, utxo_size, &err);
  leveldb_writeoptions_destroy(woptions);
  dump_buf("", "KEY: ", db_key, key_len);

  if (err != NULL) {
    fprintf(stderr, "Write fail.\n");
    return(1);
  }

  leveldb_free(err); err = NULL;
  return 0;
}

/**
 * @brief WHEN DO WE EFREE found utxo if it returns false
 * 
 * @param found_utxo 
 * @param tx 
 * @param vout 
 * @return int 
 */
int utxo_pool_find_leveldb(UTXO **found_utxo, Transaction *tx, unsigned int vout){
  if(check_if_db_loaded(&utxo_pool_db, utxo_pool_path) != 0){
    return 5;
  }

  unsigned char *db_key;
  size_t key_len;
  char *err = NULL;
  size_t read_len;
  char *read;
  //Make Key
  if(make_utxo_pool_key(&db_key, &key_len, tx, vout) != 0){
    return 2;
  }

  leveldb_readoptions_t *roptions = leveldb_readoptions_create();
  read = leveldb_get(utxo_pool_db, roptions, db_key, key_len, &read_len, &err);
  leveldb_readoptions_destroy(roptions);
  if (err != NULL) {
    fprintf(stderr, "Read fail.\n");
    return 3;
  }
  if(read == NULL){
    return 1;
  }

  *found_utxo = deser_utxo((unsigned char*) read);
  return 0;
}

int utxo_pool_remove_leveldb(Transaction *tx, unsigned int vout){
  if(check_if_db_loaded(&utxo_pool_db, utxo_pool_path) != 0){
    return 5;
  }
  unsigned char *db_key;
  size_t key_len;
  char *err = NULL;
  size_t read_len;
  char *read;
  //Make Key
  if(make_utxo_pool_key(&db_key, &key_len, tx, vout) != 0){
    return 2;
  }
  leveldb_writeoptions_t *woptions = leveldb_writeoptions_create();
  leveldb_delete(utxo_pool_db, woptions, db_key, key_len, &err);
  leveldb_writeoptions_destroy(woptions);
  if (err != NULL) {
    fprintf(stderr, "Delete fail.\n");
    return(3);
  }
  leveldb_free(err); err = NULL;
  return 0;
}

int utxo_pool_count(int *num_entries){
  if(check_if_db_loaded(&utxo_pool_db, utxo_pool_path) != 0){
    return 5;
  }
  int count = 0;
  char *err = NULL;
  leveldb_readoptions_t *roptions;
  roptions = leveldb_readoptions_create();
  leveldb_iterator_t *iter = leveldb_create_iterator(utxo_pool_db, roptions);

  for (leveldb_iter_seek_to_first(iter); leveldb_iter_valid(iter); leveldb_iter_next(iter))
  {
    count++;
      // size_t key_len, value_len;
      // unsigned const char *key_ptr = (unsigned const char*) leveldb_iter_key(iter, &key_len);
      // unsigned const char *value_ptr = (unsigned const char*) leveldb_iter_value(iter, &value_len);

      // dump_buf("", "KEY: ", key_ptr,key_len);


      // UTXO *read_utxo = deser_utxo((unsigned char*)value_ptr);
      // print_utxo(read_utxo, "");
      // /* Prints some binary noise with the data */
  }
  leveldb_iter_destroy(iter);
  leveldb_readoptions_destroy(roptions);
  leveldb_free(err); 
  *num_entries = count;
  return 0;
}

UTXO *utxo_pool_add(Transaction *tx, unsigned int vout) {
  UTXOPool *new_entry, *found_entry;
  UTXO *utxo;

  new_entry = malloc(sizeof(UTXOPool));

  // Build key
  memset(&(new_entry->id), 0, sizeof(UTXOPoolKey));
  hash_tx(new_entry->id.tx_hash, tx);
  new_entry->id.vout = vout;

  // Build new UTXO
  utxo = malloc(sizeof(UTXO));
  utxo->amt = tx->outputs[vout].amt;
  memcpy(utxo->public_key_hash, tx->outputs[vout].public_key_hash, PUB_KEY_HASH_LEN);

  new_entry->utxo = utxo;

  found_entry = utxo_pool_find_node_key(&(new_entry->id));
  if (found_entry == NULL) {
    HASH_ADD(hh, utxo_pool, id, sizeof(UTXOPoolKey), new_entry);
    return utxo;
  } else {
    free(new_entry->utxo);
    free(new_entry);
    return NULL;
  }
}

UTXO *utxo_pool_remove(unsigned char *tx_hash, unsigned int vout) {
  UTXOPool *entry;
  UTXO *utxo;

  entry = utxo_pool_find_node(tx_hash, vout);
  if (entry != NULL) {
    utxo = entry->utxo;
    HASH_DEL(utxo_pool, entry);
    free(entry);
    return utxo;
  }

  return NULL;
}

UTXO *utxo_pool_find(unsigned char *tx_hash, unsigned int vout) {
  UTXOPool *found_entry;

  found_entry = utxo_pool_find_node(tx_hash, vout);
  if (found_entry != NULL) {
    return found_entry->utxo;
  }

  return NULL;
}

UTXOPool *utxo_pool_find_node(unsigned char *tx_hash, unsigned int vout) {
  UTXOPoolKey key;

  memset(&key, 0, sizeof(UTXOPoolKey));
  memcpy(key.tx_hash, tx_hash, TX_HASH_LEN);
  key.vout = vout;

  return utxo_pool_find_node_key(&key);
}

UTXOPool *utxo_pool_find_node_key(UTXOPoolKey *key) {
  UTXOPool *found_entry;
  HASH_FIND(hh, utxo_pool, key, sizeof(UTXOPoolKey), found_entry);
  return found_entry;
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
  UTXOPool *s;
  printf("%sUTXO Hashmap items(%i):\n", prefix, HASH_COUNT(utxo_pool));
  for (s = utxo_pool; s != NULL; s = s->hh.next) {
    print_UTXOPOOL(s, prefix);
  }
  free(sub_prefix);
}

void print_UTXOPOOL(UTXOPool *utxo_pool_node, char *prefix){
  char *sub_prefix = malloc(strlen(prefix)+strlen(PRINT_TAB)+1);
  strcpy(sub_prefix, prefix);
  strcat(sub_prefix, PRINT_TAB);

  printf("%shashmap_id:\n", prefix);
  dump_buf(sub_prefix, "hashmap_id:", utxo_pool_node->id.tx_hash, TX_HASH_LEN);
  printf("%svout: %i\n", sub_prefix, utxo_pool_node->id.vout);
  print_utxo(utxo_pool_node->utxo, prefix);
  free(sub_prefix);
}
