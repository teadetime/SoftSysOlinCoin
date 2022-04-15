#include "leveldb/c.h"
#include <stdio.h>
#include "utxo_pool.h"
#include "init_db.h"
#include "crypto.h"
#include "ser_tx.h"

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
  out[0].amt = 99;
  tx->num_inputs = 1;
  tx->inputs = in;
  tx->num_outputs = 1;
  tx->outputs = out;

  return tx;
}

int main() {
  int ret;
  UTXO *found = NULL;
  utxo_pool_init_leveldb();
  
  // leveldb_t *db1 = NULL;
  // int success = open_or_create_db(&db1, utxo_pool_path);

  Transaction *tx;
  tx = _make_tx();
  ret = utxo_pool_add_leveldb(tx, 0);
  printf("Add Return Value: %i\n", ret);
  ret = utxo_pool_find_leveldb(&found, tx, 0);
  
  printf("Find Return Value: %i\n", ret);
  // if(ret == 0){
  //   print_utxo(found, "");
  //   free(found);
  // }
  ret = utxo_pool_remove_leveldb(tx, 0);

  int entries;
  int count_ret = utxo_pool_count(&entries);
  printf("count Return Value: %i, counted: %i\n", count_ret, entries);
  return(0);
}
