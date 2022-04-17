#include "init_globals.h"

#include "blockchain.h"
#include "utxo_pool.h"
#include "mempool.h"

void node_init() {
  blockchain_init_leveldb();
  utxo_pool_init_leveldb();
}

void miner_init() {
  mempool_init();
}
