#include <stdio.h>
#include "blockchain.h"
#include "crypto.h"
#include "init_db.h"
#include "ser_block.h"

int blockchain_init_leveldb(){
  int init_ret = init_db(&blockchain_db, &blockchain_path, "/blockchain");
  if(init_ret != 0){
    return 5;
  }
  Block* genesis_block;
  chain_height = 0;

  genesis_block = malloc(sizeof(Block));

  // Initialize with a totally empty block
  genesis_block->num_txs = 0;
  genesis_block->txs = NULL;

  genesis_block->header.timestamp = 0;
  genesis_block->header.nonce = 0;
  memset(genesis_block->header.all_tx, 0, TX_HASH_LEN);
  memset(genesis_block->header.prev_header_hash, 0, BLOCK_HASH_LEN);

  int ret = blockchain_add_leveldb(genesis_block);
  if(ret != 0){
    free(genesis_block);
    return 3;
  }
  return 0;
}

int blockchain_add_leveldb(Block *block){
  /* if(check_if_db_loaded(&blockchain_db, blockchain_path) != 0){ */
  /*   return 5; */
  /* } */

  unsigned char db_key[BLOCK_HASH_LEN];
  hash_blockheader(db_key, &(block->header));

  size_t key_len = BLOCK_HASH_LEN;
  char *err = NULL;

  size_t block_size;
  unsigned char *serialized_block = ser_block_alloc(&block_size, block);
  if(!serialized_block){
    return 3;
  }

  leveldb_writeoptions_t *woptions = leveldb_writeoptions_create();
  leveldb_put(blockchain_db, woptions, db_key, key_len, serialized_block, block_size, &err);
  leveldb_writeoptions_destroy(woptions);
  dump_buf("", "KEY: ", db_key, key_len);
  if (err != NULL) {
    fprintf(stderr, "Write Block fail: %s\n", err);
    leveldb_free(err);
    return 1;
  }

  chain_height += 1;
  memcpy(top_block_header_hash, db_key, BLOCK_HASH_LEN);
  leveldb_free(err);
  return 0;
}

int blockchain_find_leveldb(Block **found_block, unsigned char *block_hash){
  if(check_if_db_loaded(&blockchain_db, blockchain_path) != 0){
    return 5;
  }
  char *err = NULL;
  size_t read_len;
  char *read = NULL;

  leveldb_readoptions_t *roptions = leveldb_readoptions_create();
  read = leveldb_get(blockchain_db, roptions, block_hash, BLOCK_HASH_LEN, &read_len, &err);
  leveldb_readoptions_destroy(roptions);

  if (err != NULL) {
    fprintf(stderr, "Read fail: %s\n", err);
    leveldb_free(err);
    return 3;
  }
  leveldb_free(err);
  if(read == NULL){
    return 1;
  }

  *found_block = deser_block_alloc(NULL, (unsigned char*) read);
  free(read);
  if(!*found_block){
    return 4;
  }
  return 0;
}

int blockchain_remove_leveldb(unsigned char *block_hash){
  if(check_if_db_loaded(&blockchain_db, blockchain_path) != 0){
    return 5;
  }
  char *err = NULL;
  leveldb_writeoptions_t *woptions = leveldb_writeoptions_create();
  leveldb_delete(blockchain_db, woptions, block_hash, BLOCK_HASH_LEN, &err);
  leveldb_writeoptions_destroy(woptions);
  if (err != NULL) {
    fprintf(stderr, "Delete fail: %s\n", err);
    leveldb_free(err);
    return(3);
  }
  leveldb_free(err);
  return 0;
}

int blockchain_count(unsigned int *num_entries){
  return db_count(blockchain_db, blockchain_path, num_entries);
}

void print_blockchain_hashmap(char *prefix){
  char *sub_prefix = malloc(strlen(prefix)+strlen(PRINT_TAB)+1);
  strcpy(sub_prefix, prefix);
  strcat(sub_prefix, PRINT_TAB);
  unsigned int num_items;
  blockchain_count(&num_items);
  printf("%sBlockchain Hashmap items(%i):\n", prefix, num_items);

  char *err = NULL;
  leveldb_readoptions_t *roptions;
  roptions = leveldb_readoptions_create();
  leveldb_iterator_t *iter = leveldb_create_iterator(blockchain_db, roptions);

  for (leveldb_iter_seek_to_first(iter); leveldb_iter_valid(iter); leveldb_iter_next(iter))
  {
      size_t key_len, value_len;
      unsigned const char *key_ptr = (unsigned const char*) leveldb_iter_key(iter, &key_len);
      unsigned const char *value_ptr = (unsigned const char*) leveldb_iter_value(iter, &value_len);

      printf("%shashmap_id:\n", prefix);
      dump_buf(sub_prefix, "block_hash:", key_ptr, BLOCK_HASH_LEN);
      

      Block *read_block = deser_block_alloc(NULL, (unsigned char*)value_ptr);
      print_block(read_block, prefix);
      // /* Prints some binary noise with the data */
  }
  leveldb_iter_destroy(iter);
  leveldb_readoptions_destroy(roptions);
  leveldb_free(err); 
  free(sub_prefix);
}

void pretty_print_blockchain_hashmap(){
  unsigned int num_items;
  blockchain_count(&num_items);
  printf("%i items...\n", num_items);
  printf(LINE_BREAK);

  char *err = NULL;
  leveldb_readoptions_t *roptions;
  roptions = leveldb_readoptions_create();
  leveldb_iterator_t *iter = leveldb_create_iterator(blockchain_db, roptions);

  for (leveldb_iter_seek_to_first(iter); leveldb_iter_valid(iter); leveldb_iter_next(iter))
  {
      size_t key_len, value_len;
      unsigned const char *key_ptr = (unsigned const char*) leveldb_iter_key(iter, &key_len);
      unsigned const char *value_ptr = (unsigned const char*) leveldb_iter_value(iter, &value_len);

      dump_buf("", "Block_Hash:", key_ptr, BLOCK_HASH_LEN);
      

      Block *read_block = deser_block_alloc(NULL, (unsigned char*)value_ptr);
      pretty_print_block_header(&read_block->header, PRINT_TAB);
      // /* Prints some binary noise with the data */
  }
  leveldb_iter_destroy(iter);
  leveldb_readoptions_destroy(roptions);
  leveldb_free(err); 
}
