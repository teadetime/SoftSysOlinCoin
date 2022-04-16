#include <stdio.h>
#include "blockchain.h"
#include "crypto.h"
#include "init_db.h"
#include "ser_block.h"

void blockchain_init() {
  Block* genesis_block;
  chain_height = 0;
  blockchain = NULL;  // Must always start at NULL

  genesis_block = malloc(sizeof(Block));

  // Initialize with a totally empty block
  genesis_block->num_txs = 0;
  genesis_block->txs = NULL;

  genesis_block->header.timestamp = 0;
  genesis_block->header.nonce = 0;
  memset(genesis_block->header.all_tx, 0, TX_HASH_LEN);
  memset(genesis_block->header.prev_header_hash, 0, BLOCK_HASH_LEN);

  blockchain_add(genesis_block);
}

int blockchain_init_leveldb(){
  if(init_db(&blockchain_db, &blockchain_path, "/blockchain") != 0){
    return 5;
  }
  Block* genesis_block;
  chain_height = 0;
  blockchain = NULL;  // Must always start at NULL

  genesis_block = malloc(sizeof(Block));

  // Initialize with a totally empty block
  genesis_block->num_txs = 0;
  genesis_block->txs = NULL;

  genesis_block->header.timestamp = 0;
  genesis_block->header.nonce = 0;
  memset(genesis_block->header.all_tx, 0, TX_HASH_LEN);
  memset(genesis_block->header.prev_header_hash, 0, BLOCK_HASH_LEN);

  blockchain_add_leveldb(genesis_block);
}

int blockchain_add_leveldb(Block *block){
  if(check_if_db_loaded(&blockchain_db, blockchain_path) != 0){
    return 5;
  }

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

Block *blockchain_add(Block *block) {
  BlockChain *new_entry, *found_entry;

  new_entry = malloc(sizeof(BlockChain));
  hash_blockheader(new_entry->id, &(block->header));
  new_entry->block = block;

  found_entry = blockchain_find_node(new_entry->id);
  if (found_entry == NULL) {
    HASH_ADD(hh, blockchain, id, BLOCK_HASH_LEN, new_entry);
    chain_height += 1;
    hash_blockheader(top_block_header_hash, &(block->header));
    return block;
  }
  free(new_entry);
  return NULL;
}

Block *blockchain_remove(unsigned char *header_hash) {
  BlockChain *entry;
  Block *block;

  entry = blockchain_find_node(header_hash);
  if (entry != NULL) {
    block = entry->block;
    HASH_DEL(blockchain, entry);
    free(entry);
    chain_height -= 1;
    return block;
  }

  return NULL;
}

Block *blockchain_find(unsigned char *header_hash) {
  BlockChain *found_entry;

  found_entry = blockchain_find_node(header_hash);
  if (found_entry != NULL) {
    return found_entry->block;
  }

  return NULL;
}

BlockChain *blockchain_find_node(unsigned char *header_hash) {
  BlockChain *found_entry;
  HASH_FIND(hh, blockchain, header_hash, BLOCK_HASH_LEN, found_entry);
  return found_entry;
}

void print_blockchain(BlockChain *blockchain_node, char *prefix){
  dump_buf(prefix, "hashmap_id: ", blockchain_node->id, BLOCK_HASH_LEN);
  print_block(blockchain_node->block, prefix);
}


void print_blockchain_hashmap(char *prefix){
  char *sub_prefix = malloc(strlen(prefix)+strlen(PRINT_TAB)+1);
  strcpy(sub_prefix, prefix);
  strcat(sub_prefix, PRINT_TAB);

  BlockChain *s;
  printf("%sBlockchain Hashmap items(%i):\n", prefix, HASH_COUNT(blockchain));
  for (s = blockchain; s != NULL; s = s->hh.next) {
    print_blockchain(s, sub_prefix);
  }
  free(sub_prefix);
}

void pretty_print_blockchain(BlockChain *blockchain_node, char *prefix){
  dump_buf(prefix, "Block Hash: ", blockchain_node->id, BLOCK_HASH_LEN);
  pretty_print_block_header(&blockchain_node->block->header, prefix);
  printf(LINE_BREAK);
}

void pretty_print_blockchain_hashmap(){
  BlockChain *s;
  printf("%i items...\n", HASH_COUNT(blockchain));
  printf(LINE_BREAK);
  for (s = blockchain; s != NULL; s = s->hh.next) {
    pretty_print_blockchain(s, "");
  }
}
