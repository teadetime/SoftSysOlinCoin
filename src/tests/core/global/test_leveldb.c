#include "leveldb/c.h"
#include <stdio.h>
#include "utxo_pool.h"
#include "init_db.h"
#include "crypto.h"

Transaction *_make_tx() {
  Transaction *tx;
  Input *in;
  Output *out;
  mbedtls_ecdsa_context *key_pair;

  tx = malloc(sizeof(Transaction));
  in = malloc(sizeof(Input));
  out = malloc(sizeof(Output));

  memset(in, 0, sizeof(Input));
  key_pair = gen_keys();
  in->pub_key = &(key_pair->private_Q);

  memset(out, 0, sizeof(Output));

  tx->num_inputs = 1;
  tx->inputs = in;
  tx->num_outputs = 1;
  tx->outputs = out;

  return tx;
}

int main() {

  utxo_pool_init_leveldb();
  
  // leveldb_t *db1 = NULL;
  // int success = open_or_create_db(&db1, utxo_pool_path);

  Transaction *tx;
  tx = _make_tx();
  int ret = utxo_pool_add_leveldb(tx, 1);
    char *err = NULL;
    leveldb_readoptions_t *roptions;
    roptions = leveldb_readoptions_create();
    leveldb_iterator_t *iter = leveldb_create_iterator(utxo_pool_db, roptions);

    for (leveldb_iter_seek_to_first(iter); leveldb_iter_valid(iter); leveldb_iter_next(iter))
    {
        size_t key_len, value_len;
        unsigned const char *key_ptr = (unsigned const char*) leveldb_iter_key(iter, &key_len);
        unsigned const char *value_ptr = (unsigned const char*) leveldb_iter_value(iter, &value_len);

        dump_buf("", "KEY: ", key_ptr,key_len);
        /* Prints some binary noise with the data */
        printf("%s\n", value_ptr);
    }
    leveldb_iter_destroy(iter);
    leveldb_free(err); 
    err = NULL;

    // leveldb_t *db;
    // leveldb_options_t *options;
    // leveldb_readoptions_t *roptions;
    // leveldb_writeoptions_t *woptions;
    // //char *err = NULL;
    // char *read;
    // size_t read_len;

    // /******************************************/
    // /* OPEN */

    // options = leveldb_options_create();
    // leveldb_options_set_create_if_missing(options, 1);
    // leveldb_options_set_compression(options, 0);
    // leveldb_options_set_cache(options, leveldb_cache_create_lru(100 * 1048576));
    // db = leveldb_open(options, "testdb", &err);

    // if (err != NULL) {
    //   fprintf(stderr, "Open fail.\n");
    //   return(1);
    // }

    // /* reset error var */
    // leveldb_free(err); err = NULL;

    // /******************************************/
    // /* WRITE */

    // woptions = leveldb_writeoptions_create();
    // leveldb_put(db, woptions, "key", 3, "value", 5, &err);

    // if (err != NULL) {
    //   fprintf(stderr, "Write fail.\n");
    //   return(1);
    // }

    // leveldb_free(err); err = NULL;

    // /******************************************/
    // /* READ */

    // roptions = leveldb_readoptions_create();
    // read = leveldb_get(db, roptions, "key", 3, &read_len, &err);

    // if (err != NULL) {
    //   fprintf(stderr, "Read fail.\n");
    //   return(1);
    // }

    // printf("key: %s\n", read);

    // leveldb_free(err); err = NULL;

    // /******************************************/
    // /* DELETE */

    // leveldb_delete(db, woptions, "key", 3, &err);

    // if (err != NULL) {
    //   fprintf(stderr, "Delete fail.\n");
    //   return(1);
    // }

    // leveldb_free(err); err = NULL;

    // /******************************************/
    // /* CLOSE */

    // leveldb_close(db);

    // /******************************************/
    // /* DESTROY */

    // leveldb_destroy_db(options, "testdb", &err);

    // if (err != NULL) {
    //   fprintf(stderr, "Destroy fail.\n");
    //   return(1);
    // }

    // leveldb_free(err); err = NULL;


    return(0);
}
