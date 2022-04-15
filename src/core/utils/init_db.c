#include "leveldb/c.h"
#include <stdlib.h>
#include <stdio.h>
#include "string.h"

int set_db_path(char *dest[], char *name){
  if(!getenv("HOME")){
        exit(1);
  }
  char* home_path = getenv("HOME");
  *dest = strcat(home_path, name);
  return 1;
}

int open_or_create_db(leveldb_t **db, char *path){
  leveldb_options_t *options;
  char *err = NULL;
  int ret = 0;
  options = leveldb_options_create();
  leveldb_options_set_create_if_missing(options, 1);
  leveldb_options_set_compression(options, 0);  // Turn Off compression
  //leveldb_options_set_cache(options, leveldb_cache_create_lru(100 * 1048576)); // Set large cache size@
  *db = leveldb_open(options, path, &err);
  if (err != NULL) {
    //fprintf(stderr, "Open fail.\n");
    ret = 1;
  }
  /* reset error var */
  leveldb_options_destroy(options);
  leveldb_free(err); err = NULL;
  return ret;
}

int check_if_db_loaded(leveldb_t **db, char *path){
  if(*db == NULL){
    fprintf(stderr, "Database in use by other process!\n");
    // Try to open it again and see if that works
    if(open_or_create_db(db, path) != 0){
      return 1;
    }
  }
  return 0;
}


int destroy_db(leveldb_t *db, char *name){
  /**Note this could be weird, maybe we don't care about closing the database */
  leveldb_close(db);
  leveldb_options_t *options;
  char *err = NULL;
  int ret = 0;
  options = leveldb_options_create();
 
  /* DESTROY */
  leveldb_destroy_db(options, name, &err);
  if (err != NULL) {
    //fprintf(stderr, "Open fail.\n");
    ret = 1;
  }
  /* reset error var */
  leveldb_options_destroy(options);
  leveldb_free(err); err = NULL;
  return ret;
}