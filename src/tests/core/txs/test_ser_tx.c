#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <setjmp.h>
#include <cmocka.h>

#include "base_tx.h"
#include "ser_tx.h"
#include "crypto.h"
#include "fixtures_tx.h"

static void test_ser_tx(void **state) {
  ssize_t written_ser_tx, read_ser_tx;
  Transaction *tx, *desered_tx;
  unsigned char *sered_tx;
  unsigned char buf_1[PUB_KEY_SER_LEN], buf_2[PUB_KEY_SER_LEN];
  unsigned char output_hash[TX_HASH_LEN], deser_output_hash[TX_HASH_LEN];

  tx = *state;
  sered_tx = ser_tx_alloc(&read_ser_tx, tx);
  desered_tx = deser_tx_alloc(&written_ser_tx, sered_tx);

  assert_true(read_ser_tx == (ssize_t)size_ser_tx(tx));
  assert_true(read_ser_tx == written_ser_tx);
  assert_true(tx->num_inputs == desered_tx->num_inputs);
  assert_true(tx->num_outputs == desered_tx->num_outputs);

  for (size_t i = 0; i < tx->num_inputs; i++) {
    assert_true(
      tx->inputs[i].prev_utxo_output == desered_tx->inputs[i].prev_utxo_output
    );
    assert_memory_equal(
      tx->inputs[i].prev_tx_id,
      desered_tx->inputs[i].prev_tx_id,
      TX_HASH_LEN
    );
    assert_memory_equal(
      tx->inputs[i].signature,
      desered_tx->inputs[i].signature,
      SIGNATURE_LEN
    );
    assert_true(tx->inputs[i].sig_len == desered_tx->inputs[i].sig_len);

    ser_pub_key(buf_1, tx->inputs[i].pub_key);
    ser_pub_key(buf_2, desered_tx->inputs[i].pub_key);
    assert_memory_equal(buf_1, buf_2, PUB_KEY_SER_LEN);
  }

  for (size_t i = 0; i < tx->num_outputs; i++) {
    assert_true(tx->outputs[i].amt == desered_tx->outputs[i].amt);
    assert_memory_equal(
      tx->outputs[i].public_key_hash,
      desered_tx->outputs[i].public_key_hash,
      PUB_KEY_HASH_LEN
    );
  }

  hash_tx(output_hash, tx);
  hash_tx(deser_output_hash, desered_tx);
  assert_memory_equal(output_hash, deser_output_hash, TX_HASH_LEN);

  free(sered_tx);
  free(desered_tx);
}

static void test_ser_tx_pad(void **state) {
  ssize_t read_ser_tx;
  Transaction *tx, *pad_tx;
  unsigned char *sered_tx, *sered_tx_2;
  size_t tx_ser_size;

  tx = *state;
  sered_tx = ser_tx_alloc(&read_ser_tx, tx);

  // Ensure that we fill the entire buffer
  tx_ser_size = size_ser_tx(tx);
  sered_tx_2 = malloc(tx_ser_size);
  for (int i = 0; i < 5; i++) {
    memset(sered_tx_2, i, tx_ser_size);
    ser_tx(sered_tx_2, tx);
    assert_memory_equal(sered_tx, sered_tx_2, UTXO_SER_LEN);
  }

  pad_tx = malloc(sizeof(Transaction));
  for (int i = 0; i < 5; i++) {
    memset(pad_tx, i, sizeof(Transaction));

    // Copy everything but padding bytes
    // Fill new mallocs with random bytes to simulate new paddinig
    pad_tx->num_inputs = tx->num_inputs;
    pad_tx->inputs = malloc(tx->num_inputs * sizeof(Input));
    memset(pad_tx->inputs, i, tx->num_inputs * sizeof(Input));
    for(unsigned int j = 0; j < tx->num_inputs; j++){
      pad_tx->inputs[j].pub_key = malloc(sizeof(mbedtls_ecp_point));
      memset(pad_tx->inputs[j].pub_key, i, sizeof(mbedtls_ecp_point));
      mbedtls_ecp_point_init(pad_tx->inputs[j].pub_key);
      mbedtls_ecp_copy(
        pad_tx->inputs[j].pub_key,
        tx->inputs[j].pub_key
      );

      pad_tx->inputs[j].prev_utxo_output = tx->inputs[j].prev_utxo_output;
      memcpy(
        pad_tx->inputs[j].prev_tx_id,
        tx->inputs[j].prev_tx_id,
        TX_HASH_LEN
      );

      pad_tx->inputs[j].sig_len = tx->inputs[j].sig_len;
      memcpy(
        pad_tx->inputs[j].signature,
        tx->inputs[j].signature,
        pad_tx->inputs[j].sig_len
      );
    }

    pad_tx->num_outputs = tx->num_outputs;
    pad_tx->outputs = malloc(tx->num_outputs * sizeof(Output));
    memset(pad_tx->outputs, i, tx->num_outputs * sizeof(Output));
    for (unsigned int j = 0; j < tx->num_outputs; j++) {
      pad_tx->outputs[j].amt = tx->outputs[j].amt;
      memcpy(
        pad_tx->outputs[j].public_key_hash,
        tx->outputs[j].public_key_hash,
        PUB_KEY_HASH_LEN
      );
    }

    ser_tx(sered_tx_2, pad_tx);
    assert_memory_equal(sered_tx, sered_tx_2, tx_ser_size);
  }

  free(pad_tx);
  free(sered_tx);
  free(sered_tx_2);
}

static void test_ser_utxo(void **state) {
  UTXO *utxo, *desered_utxo;
  unsigned char *sered_utxo;
  ssize_t read_ser_utxo, written_ser_utxo;

  utxo = *state;
  sered_utxo = ser_utxo_alloc(&read_ser_utxo, utxo);
  desered_utxo = deser_utxo_alloc(&written_ser_utxo, sered_utxo);

  assert_true(read_ser_utxo == UTXO_SER_LEN);
  assert_true(read_ser_utxo == written_ser_utxo);
  assert_true(utxo->amt == desered_utxo->amt);
  assert_memory_equal(
    utxo->public_key_hash,
    desered_utxo->public_key_hash,
    PUB_KEY_HASH_LEN
  );

  free(sered_utxo);
  free(desered_utxo);
}

static void test_ser_utxo_pad(void **state) {
  UTXO *utxo, *pad_utxo;
  unsigned char *sered_utxo;
  unsigned char sered_utxo_2[UTXO_SER_LEN];
  ssize_t read_ser_utxo;

  utxo = *state;
  sered_utxo = ser_utxo_alloc(&read_ser_utxo, utxo);

  // Ensure that we fill the entire buffer
  for (int i = 0; i < 5; i++) {
    memset(sered_utxo_2, i, UTXO_SER_LEN);
    ser_utxo(sered_utxo_2, utxo);
    assert_memory_equal(sered_utxo, sered_utxo_2, UTXO_SER_LEN);
  }

  // Ensure we don't serialize padding bytes
  pad_utxo = malloc(sizeof(UTXO));
  for (int i = 0; i < 5; i++) {
    memset(pad_utxo, i, sizeof(UTXO));
    pad_utxo->amt = utxo->amt;
    memcpy(pad_utxo->public_key_hash, utxo->public_key_hash, PUB_KEY_HASH_LEN);
    ser_utxo(sered_utxo_2, pad_utxo);
    assert_memory_equal(sered_utxo, sered_utxo_2, UTXO_SER_LEN);
  }

  free(pad_utxo);
  free(sered_utxo);
}

int main() {
  const struct CMUnitTest tests[] = {
    cmocka_unit_test_setup_teardown(
      test_ser_tx,
      fixture_setup_unlinked_tx,
      fixture_teardown_unlinked_tx
    ),
    cmocka_unit_test_setup_teardown(
      test_ser_tx_pad,
      fixture_setup_unlinked_tx,
      fixture_teardown_unlinked_tx
    ),
    cmocka_unit_test_setup_teardown(
      test_ser_utxo,
      fixture_setup_unlinked_utxo,
      fixture_teardown_unlinked_utxo
    ),
    cmocka_unit_test_setup_teardown(
      test_ser_utxo_pad,
      fixture_setup_unlinked_utxo,
      fixture_teardown_unlinked_utxo
    )
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
