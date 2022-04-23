#include "init_globals.h"

#include "blockchain.h"
#include "utxo_pool.h"
#include "mempool.h"
#include "init_db.h"

void node_init(char *db_env) {
  // Read from file for chain height and top block hash
  blockchain_init_leveldb(db_env);
  utxo_pool_init_leveldb(db_env);
}

void miner_init() {
  mempool_init();
}
