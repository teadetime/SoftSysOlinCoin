#include "init_globals.h"

#include "blockchain.h"
#include "utxo_pool.h"
#include "mempool.h"
#include "init_db.h"

void node_init() {
  // Read from file for chain height and top block hash
  blockchain_init_leveldb(PROD_DB_LOC);
  utxo_pool_init_leveldb(PROD_DB_LOC);
}

void miner_init() {
  mempool_init();
}
