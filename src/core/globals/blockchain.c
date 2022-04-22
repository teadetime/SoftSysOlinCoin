#include <stdio.h>
#include "blockchain.h"
#include "crypto.h"
#include "init_db.h"
#include "ser_block.h"

int read_top_hash(unsigned char *dest){
  char path[strlen(blockchain_path) + strlen(TOP_BLOCK_HASH_FILE) + 1];
  strcpy(path, blockchain_path);
  strcat(path, TOP_BLOCK_HASH_FILE);
  printf("%s\n", path);
  FILE *fp_hash = fopen(path, "r");
  if(!fp_hash){
    return 1;
  }
  fgets(dest, BLOCK_HASH_LEN+1, fp_hash); // Add for null character
  fclose(fp_hash);
  return 0;
}

int write_top_hash(){
  char path[strlen(blockchain_path) + strlen(TOP_BLOCK_HASH_FILE) + 1];
  strcpy(path, blockchain_path);
  strcat(path, TOP_BLOCK_HASH_FILE);
  FILE *fp_hash = fopen(path, "w+");
  if(!fp_hash){
    exit(0);
  }
  fwrite(top_block_header_hash, sizeof(unsigned char), BLOCK_HASH_LEN, fp_hash);
  fclose(fp_hash);
  return 0;
}

int read_chain(){
  int i = -1;
  char path[strlen(blockchain_path) + strlen(CHAIN_HEIGHT_FILE) + 1];
  strcpy(path, blockchain_path);
  strcat(path, CHAIN_HEIGHT_FILE);
  printf("%s\n", path);
  FILE *fp_chain = fopen(path, "r");
  if(!fp_chain){
    return i;
  }  
  fscanf(fp_chain, "%d", &i);
  fclose(fp_chain);
  return i;
}

int write_chain(){
  char path[strlen(blockchain_path) + strlen(CHAIN_HEIGHT_FILE) + 1];
  strcpy(path, blockchain_path);
  strcat(path, CHAIN_HEIGHT_FILE);
  FILE *fp_chain = fopen(path, "w+");
  if(!fp_chain){
    exit(0);
  }
  fprintf(fp_chain,"%li", chain_height);
  fclose(fp_chain);
  return 0;
}


int blockchain_init_leveldb(char *db_env){
  int init_ret = init_db(&blockchain_db, &blockchain_path, db_env, "/blockchain");
  if(init_ret != 0){
    return 5;
  }
  int use_existing_db = 0;
  // Check if we are using an existing DB
  unsigned char file_top_block_hash[BLOCK_HASH_LEN];
  int read_hash = read_top_hash(file_top_block_hash);
  int file_chain_height = read_chain();
  
  if(read_hash == 0 && file_chain_height != -1){
    Block *test_top_block;
    int found_top = blockchain_find_leveldb(&test_top_block, file_top_block_hash);
    if(found_top == 0){
      free(test_top_block);
    }
    unsigned int num_entries;
    blockchain_count(&num_entries);
    if(found_top == 0 && num_entries >= file_chain_height){
      use_existing_db = 1;
    }
  }

  if(use_existing_db){
    chain_height = file_chain_height;
    memcpy(top_block_header_hash, file_top_block_hash, BLOCK_HASH_LEN);
  }
  else{
    Block genesis_block;
    chain_height = 0;
    // Initialize with a totally empty block
    genesis_block.num_txs = 0;
    genesis_block.txs = NULL;

    genesis_block.header.timestamp = 0;
    genesis_block.header.nonce = 0;
    memset(genesis_block.header.all_tx, 0, TX_HASH_LEN);
    memset(genesis_block.header.prev_header_hash, 0, BLOCK_HASH_LEN);

    int ret = blockchain_add_leveldb(&genesis_block);
    if(ret != 0){
      return 3;
    }
  }
  return 0;
}

int blockchain_add_leveldb(Block *block){
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
  write_chain("");
  memcpy(top_block_header_hash, db_key, BLOCK_HASH_LEN);
  write_top_hash("");
  leveldb_free(err);
  return 0;
}

int blockchain_find_leveldb(Block **found_block, unsigned char *block_hash){
  char *err = NULL;
  size_t read_len;
  char *read = NULL;

  leveldb_readoptions_t *roptions = leveldb_readoptions_create();
  read = leveldb_get(blockchain_db, roptions, block_hash, BLOCK_HASH_LEN, &read_len, &err);
  leveldb_readoptions_destroy(roptions);

  if (err != NULL) {
    fprintf(stderr, "Read fail: %s\n", err);
    leveldb_free(err);
    *found_block = NULL;
    return 3;
  }
  leveldb_free(err);
  if(read == NULL){
    *found_block = NULL;
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
