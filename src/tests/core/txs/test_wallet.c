#include "minunit.h"
#include "wallet.h"
#include "crypto.h"
#include "ser_wallet.h"

int tests_run = 0;

TxOptions *_make_options() {
  TxOptions *options;
  Output *out;

  options = malloc(sizeof(TxOptions));
  out = malloc(sizeof(Output) * 3);
  out[0].amt = 2;
  memset(out[0].public_key_hash, 0, PUB_KEY_HASH_LEN);
  out[1].amt = 4;
  memset(out[1].public_key_hash, 0, PUB_KEY_HASH_LEN);
  out[2].amt = 8;
  memset(out[2].public_key_hash, 0, PUB_KEY_HASH_LEN);

  options->num_dests = 3;
  options->dests = out;
  options->tx_fee = 1;

  return options;
}

void _free_options(TxOptions *options) {
  free(options->dests);
  free(options);
}

Transaction *_make_single_out_tx(unsigned long amt) {
  Transaction *tx;
  Output *out;

  tx = malloc(sizeof(Transaction));
  out = malloc(sizeof(Output));
  out->amt = amt;

  tx->num_inputs = 0;
  tx->inputs = NULL;
  tx->num_outputs = 1;
  tx->outputs = out;

  return tx;
}

void _free_tx(Transaction *tx) {
  if (tx->inputs != NULL)
    free(tx->inputs);
  if (tx->outputs != NULL)
    free(tx->outputs);
  free(tx);
}

void _populate_wallet_pool() {
  // These txs leak..
  wallet_pool_build_add_leveldb(_make_single_out_tx(7), 0, gen_keys());
  wallet_pool_build_add_leveldb(_make_single_out_tx(10), 0, gen_keys());
}

static char *test_build_inputs() {
  TxOptions *options;
  Transaction *tx;
  mbedtls_ecdsa_context **ret_keys;
  size_t i;
  unsigned char empty_sig[SIGNATURE_LEN];

  wallet_init_leveldb(TEST_DB_LOC);
  destroy_wallet();
  wallet_init_leveldb(TEST_DB_LOC);

  _populate_wallet_pool();
  options = _make_options();
  memset(empty_sig, 0, SIGNATURE_LEN);
  tx = malloc(sizeof(Transaction));
  ret_keys = build_inputs(tx, options);

  mu_assert(
      "Options has wrong in_total",
      options->in_total = 17
  );
  mu_assert(
      "Options has wrong out_total",
      options->out_total = 15
  );
  mu_assert(
      "New tx has wrong number of inputs",
      tx->num_inputs == 2
  );

  i = 0;
  leveldb_readoptions_t *roptions2 = leveldb_readoptions_create();
  leveldb_iterator_t *iter2 = leveldb_create_iterator(wallet_pool_db, roptions2);
  for (leveldb_iter_seek_to_first(iter2); leveldb_iter_valid(iter2); leveldb_iter_next(iter2))
  {
    if(i >= tx->num_inputs){
      break;
    }

    size_t key_len, value_len;
    unsigned const char *key_ptr = (unsigned const char*) leveldb_iter_key(iter2, &key_len);
    unsigned const char *value_ptr = (unsigned const char*) leveldb_iter_value(iter2, &value_len);
    WalletEntry *read_wallet_entry = deser_wallet_entry_alloc(NULL, (unsigned char*)value_ptr);
    mu_assert(
      "Input has wrong vout",
      *(unsigned int *)(key_ptr+TX_HASH_LEN) == tx->inputs[i].prev_utxo_output
    );
    mu_assert(
      "Input has wrong tx hash",
      memcmp(key_ptr, tx->inputs[i].prev_tx_id, TX_HASH_LEN) == 0
    );
    mu_assert(
      "Input has wrong tx pub_key",
      mbedtls_ecp_point_cmp(
        &(read_wallet_entry->key_pair->private_Q),
        tx->inputs[i].pub_key
      ) == 0
    );
    mu_assert(
      "Input has wrong tx signature",
      memcmp(tx->inputs[i].signature, empty_sig, SIGNATURE_LEN) == 0
    );
    mu_assert(
      "Returned key pair private Q incorrect",
      mbedtls_ecp_point_cmp(&read_wallet_entry->key_pair->private_Q, &ret_keys[i]->private_Q) == 0
    );
    mu_assert(
      "Returned key pair private d incorrect",
      mbedtls_mpi_cmp_mpi(&read_wallet_entry->key_pair->private_d, &ret_keys[i]->private_d) == 0
    );
    mu_assert(
      "Hash map entry spent not set",
      read_wallet_entry->spent == 1
    );


    i++;
    free(read_wallet_entry);
  }
  leveldb_iter_destroy(iter2);
  leveldb_readoptions_destroy(roptions2);

  _free_options(options);
  _free_tx(tx);
  free(ret_keys);
  destroy_wallet();

  return NULL;
}

static char *test_build_outputs() {
  TxOptions *options;
  Transaction *tx;
  mbedtls_ecdsa_context *key_pair;
  unsigned char key_hash[PUB_KEY_HASH_LEN];

  options = _make_options();
  options->in_total = 17;
  options->out_total = 15;
  tx = malloc(sizeof(Transaction));

  key_pair = build_outputs(tx, options);
  hash_pub_key(key_hash, key_pair);

  for (size_t i = 0; i < options->num_dests; i++) {
    mu_assert(
        "Output has wrong amt",
        options->dests[i].amt == tx->outputs[i].amt
    );
    mu_assert(
        "Output has wrong pub key hash",
        memcmp(
          options->dests[i].public_key_hash,
          tx->outputs[i].public_key_hash,
          PUB_KEY_HASH_LEN
        ) == 0
    );
  }

  mu_assert(
      "Self output has wrong amt",
      tx->outputs[options->num_dests].amt == 2
  );
  mu_assert(
      "Self output has wrong pub key hash",
      memcmp(
        tx->outputs[options->num_dests].public_key_hash,
        key_hash,
        PUB_KEY_HASH_LEN
      ) == 0
  );

  _free_options(options);
  _free_tx(tx);
  mbedtls_ecp_keypair_free(key_pair);

  return NULL;
}

static char *test_sign_tx() {
  TxOptions *options;
  Transaction *tx;
  mbedtls_ecdsa_context **ret_keys;
  unsigned char tx_hash[TX_HASH_LEN];

  wallet_init_leveldb(TEST_DB_LOC);

  _populate_wallet_pool();
  options = _make_options();
  tx = malloc(sizeof(Transaction));
  tx->num_outputs = 0;
  tx->outputs = NULL;
  ret_keys = build_inputs(tx, options);
  hash_tx(tx_hash, tx);

  sign_tx(tx, ret_keys);
  for (size_t i = 0; i < tx->num_inputs; i++) {
    mu_assert(
        "Input signature invalid",
        validate_sig(
          tx->inputs[i].signature, tx->inputs[i].sig_len,
          tx_hash, TX_HASH_LEN,
          ret_keys[i]
        ) == 0
    );
  }

  _free_options(options);
  _free_tx(tx);
  free(*ret_keys);
  destroy_wallet();

  return NULL;
}

static char *test_build_tx() {
  TxOptions *options;
  Transaction *tx;

  wallet_init_leveldb(TEST_DB_LOC);

  _populate_wallet_pool();
  options = _make_options();
  tx = build_tx(options);

  // Only light tests here, as more detailed value-checking is performed in the
  // build_inputs and build_outputs tests.
  mu_assert(
      "Wrong number of inputs",
      tx->num_inputs = 2
  );
  mu_assert(
      "Wrong number of outputs",
      tx->num_inputs = 4
  );

  _free_options(options);
  _free_tx(tx);
  destroy_wallet();

  return NULL;
}

static char *all_tests() {
  mu_run_test(test_build_inputs);
  mu_run_test(test_build_outputs);
  mu_run_test(test_sign_tx);
  mu_run_test(test_build_tx);
  return NULL;
}

int main() {
  create_proj_folders();
  char *result = all_tests();
  if (result != NULL) {
    printf("%s\n", result);
  } else {
    printf("wallet.c passing!\n");
  }
  printf("Tests run: %d\n", tests_run);

  return result != 0;
}
