#include "leveldb/c.h"
#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include "init_db.h"
#include "constants.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

int create_folder(char *path){
  int mkdir_res = mkdir(path, 0777);
  if(mkdir_res != 0 && errno != EEXIST){
    //errors here
    return 1;   
  }
  return 0;
}

int create_proj_folders(){
  if(!getenv("HOME")){
    exit(1);
  }
  char *home_path = getenv("HOME");
  char *newPath = malloc(strlen(home_path) + strlen(LOCAL_LOCATION) + 1);
  strcpy(newPath, home_path);
  strcat(newPath, LOCAL_LOCATION);
  int ret_base = create_folder(newPath);
  if(ret_base != 0){
    return 1;
  }

  // Create prod and test folders
  char *prod = malloc(strlen(newPath) + strlen(PROD_DB_LOC) + 1);
  char *test = malloc(strlen(newPath) + strlen(TEST_DB_LOC) + 1);
  strcpy(prod, newPath);
  strcpy(test, newPath);
  free(newPath);
  strcat(prod, PROD_DB_LOC);
  strcat(test, TEST_DB_LOC);

  int ret_prod = create_folder(prod);
  free(prod);
  if(ret_prod != 0){
    return 1;
  }

  int ret_test = create_folder(test);
  free(test);
  if(ret_test != 0){
    return 1;
  }
  return 0;
}

int open_or_create_db(leveldb_t **db, char *path){
  if(*db != NULL){
    fprintf(stderr, "Database Already Open\n");
    return 0;
  }
  leveldb_options_t *options;
  char *err = NULL;
  int ret = 0;
  options = leveldb_options_create();
  leveldb_options_set_create_if_missing(options, 1);
  leveldb_options_set_compression(options, 0);  // Turn Off compression
  //leveldb_options_set_cache(options, leveldb_cache_create_lru(100 * 1048576)); // Set large cache size@
  *db = leveldb_open(options, path, &err);
  if (err != NULL) {
    fprintf(stderr, "Open fail: %s\n", err);
    ret = 1;
  }
  /* reset error var */
  leveldb_options_destroy(options);
  leveldb_free(err);
  return ret;
}

int init_db(leveldb_t **db, char **dest, char *db_env, char *name){
  if(!getenv("HOME")){
    exit(1);
  }
  char* home_path = getenv("HOME");
  *dest = malloc(strlen(home_path) + strlen(LOCAL_LOCATION) + strlen(db_env) + strlen(name) + 1);
  strcpy(*dest, home_path);
  strcat(*dest, LOCAL_LOCATION);
  strcat(*dest, db_env);
  strcat(*dest, name);

  if(open_or_create_db(db, *dest) != 0){
    return 1;
  }
  return 0;
}

int destroy_db(leveldb_t **db, char *name){
  /**Note this could be weird, maybe we don't care about closing the database */
  leveldb_close(*db);
  *db = NULL;
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

int db_count(leveldb_t *db, char *db_path, unsigned int *num_entries){
  int count = 0;
  leveldb_readoptions_t *roptions;
  roptions = leveldb_readoptions_create();
  leveldb_iterator_t *iter = leveldb_create_iterator(db, roptions);

  for (leveldb_iter_seek_to_first(iter); leveldb_iter_valid(iter); leveldb_iter_next(iter))
  {
    count++;
  }
  leveldb_iter_destroy(iter);
  leveldb_readoptions_destroy(roptions);
  *num_entries = count;
  return 0;
}