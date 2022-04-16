#pragma once
#include "leveldb/c.h"
int set_db_path(char *dest[], char *name);

int open_or_create_db(leveldb_t **db, char *path);
int init_db(leveldb_t **db, char **dest, char *name);

int check_if_db_loaded(leveldb_t **db, char *path);

int destroy_db(leveldb_t **db, char *name);

int db_count(leveldb_t *db, char *db_path, unsigned int *num_entries);