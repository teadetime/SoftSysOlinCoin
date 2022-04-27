#include "fixtures_tx.h"
#include "fixtures_global.h"
#include "utxo_pool.h"
#include "wallet_pool.h"
#include "blockchain.h"
#include "init_db.h"

#include <string.h>

static ComposedFixture *composition = NULL;

int fixture_setup_compose(void **state) {
  composition = *state;
  *state = NULL;

  int i = 0;
  while (composition->setup[i])
    (*composition->setup[i++])(state);

  return 0;
}

int fixture_teardown_compose(void **state) {
  int i = 0;
  while (composition->teardown[i])
    (*composition->teardown[i++])(state);

  composition = NULL;

  return 0;
}

int fixture_clear_utxo_pool(void **state) {
  utxo_pool_init_leveldb(TEST_DB_LOC);
  destroy_db(&utxo_pool_db, utxo_pool_path);
  return 0;
}

int fixture_setup_utxo_pool(void **state) {
  utxo_pool_init_leveldb(TEST_DB_LOC);
  return 0;
}

int fixture_teardown_utxo_pool(void **state) {
  destroy_db(&utxo_pool_db, utxo_pool_path);
  return 0;
}

int fixture_clear_wallet(void **state) {
  wallet_init_leveldb(TEST_DB_LOC);
  destroy_wallet();
  return 0;
}

int fixture_setup_wallet(void **state) {
  wallet_init_leveldb(TEST_DB_LOC);
  return 0;
}

int fixture_teardown_wallet(void **state) {
  destroy_wallet();
  return 0;
}

int fixture_clear_blockchain(void **state) {
  blockchain_init_leveldb(TEST_DB_LOC);
  destroy_blockchain();
  return 0;
}

int fixture_setup_blockchain(void **state) {
  blockchain_init_leveldb(TEST_DB_LOC);
  return 0;
}

int fixture_teardown_blockchain(void **state) {
  destroy_blockchain();
  return 0;
}
