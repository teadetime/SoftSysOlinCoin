#pragma once
#include "leveldb/c.h"
#define PROD_DB_LOC "/prod"
#define TEST_DB_LOC "/test"
#define CHAIN_HEIGHT_FILE "/chain_height.dat"
#define TOP_BLOCK_HASH_FILE "/top_block_hash.dat"

/**
 * @brief Create a folder at path location
 * 
 * @param path path to create a folder at
 * @return int 0 if success, otherwise 1
 */
int create_folder(char *path);

/**
 * @brief Creates required project folders @ 
 * $HOME/LOCAL_LOCATION/(PROD_db_LOC or TEST_db_loc)
 * 
 * @return int 0 if success, not zero otherwise
 */
int create_proj_folders();

/**
 * @brief Opens/creates a database at specified path. Resulting dtaabase allocated at **db
 * 
 * @param db double pointer to database object to open into, freeing responsibility of caller
 * @param path path to open the database
 * @return int return 0 if success, not zero if otherwise
 */
int open_or_create_db(leveldb_t **db, char *path);

/**
 * @brief Initializes a database
 * 
 * @param db double pointer for database object to be mallocced into
 * @param dest double pointer to malloc path to the database
 * @param db_env TEST_DB_LOC or PROD_DB_LOC to create databases
 * @param name name of database to initialize
 * @return int 0 if success, not zero otherwise
 */
int init_db(leveldb_t **db, char **dest, char *db_env, char *name);

/**
 * @brief Closes and destroys an entire database
 * 
 * @param db open database
 * @param name path to database to destroy
 * @return int 0 if success, not zero otherwise
 */
int destroy_db(leveldb_t **db, char *name);

/**
 * @brief Count the number of itmes in a db
 * 
 * @param db db to query
 * @param num_entries resulting count get's written into this
 * @return int 0 if success
 */
int db_count(leveldb_t *db, unsigned int *num_entries);