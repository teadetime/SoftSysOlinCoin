#pragma once
#include "leveldb/c.h"
#define PROD_DB_LOC "/prod"
#define TEST_DB_LOC "/test"
#define CHAIN_HEIGHT_FILE "/chain_height.dat"
#define TOP_BLOCK_HASH_FILE "/top_block_hash.dat"


int create_folder(char *path);

int read_chain();

int write_chain();

int create_proj_folders();

int set_db_path(char *dest[], char *name);

int open_or_create_db(leveldb_t **db, char *path);

int init_db(leveldb_t **db, char **dest, char *db_env, char *name);

int destroy_db(leveldb_t **db, char *name);

int db_count(leveldb_t *db, char *db_path, unsigned int *num_entries);