#pragma once
#include "leveldb/c.h"
int set_db_path(char *dest[], char *name);

int open_or_create_db(leveldb_t **db, char *path);

int check_if_db_loaded(leveldb_t **db, char *path);

int destroy_db(leveldb_t *db, char *name);