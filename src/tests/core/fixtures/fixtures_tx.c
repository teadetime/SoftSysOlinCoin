#include "fixtures_tx.h"
#include "crypto.h"
#include "base_tx.h"

#include <stdlib.h>
#include <string.h>

#define NUM_OUTPUTS 3
#define NUM_INPUTS 4

static void init_output(Output *output, unsigned long amt) {
  output->amt = amt;
  // Unlinked, so this does not matter. Filled with random values
  memset(&output->public_key_hash, amt + 2, PUB_KEY_HASH_LEN);
}

static mbedtls_ecdsa_context *init_input(Input *input, unsigned int vout) {
  mbedtls_ecdsa_context *keypair;

  keypair = gen_keys();
  input->pub_key = malloc(sizeof(mbedtls_ecp_point));
  mbedtls_ecp_point_init(input->pub_key);
  mbedtls_ecp_copy(input->pub_key, &keypair->private_Q);

  input->prev_utxo_output = vout;
  // Unlinked, so this does not matter. Filled with random values
  memset(&(input->prev_tx_id), vout + 5, TX_HASH_LEN);

  input->sig_len = 0;
  memset(&(input->signature), 0, SIGNATURE_LEN);

  return keypair;
}

static void sign_input(
  Input *input, mbedtls_ecdsa_context *keypair, unsigned char *tx_hash
) {
  input->sig_len = write_sig(
    input->signature, SIGNATURE_LEN,
    tx_hash, TX_HASH_LEN,
    keypair
  );
}

int fixture_setup_unlinked_tx(void **state) {
  Transaction *tx;
  unsigned char tx_hash[TX_HASH_LEN];
  mbedtls_ecdsa_context *keys[NUM_INPUTS];

  tx = malloc(sizeof(Transaction));
  tx->num_outputs = NUM_OUTPUTS;
  tx->num_inputs = NUM_INPUTS;

  tx->outputs = malloc(sizeof(Output) * (tx->num_outputs));
  tx->inputs = malloc(sizeof(Input) * (tx->num_inputs));

  init_output(&tx->outputs[0], 1);
  init_output(&tx->outputs[1], 10);
  init_output(&tx->outputs[2], 17);

  keys[0] = init_input(&tx->inputs[0], 5);
  keys[1] = init_input(&tx->inputs[1], 9);
  keys[2] = init_input(&tx->inputs[2], 0);
  keys[3] = init_input(&tx->inputs[3], 2);

  hash_tx(tx_hash, tx);
  sign_input(&tx->inputs[0], keys[0], tx_hash);
  sign_input(&tx->inputs[1], keys[1], tx_hash);
  sign_input(&tx->inputs[2], keys[2], tx_hash);
  sign_input(&tx->inputs[3], keys[3], tx_hash);

  for (int i = 0; i < NUM_INPUTS; i++)
    mbedtls_ecdsa_free(keys[i]);

  *state = tx;

  return 0;
}

int fixture_teardown_unlinked_tx(void **state) {
  free_tx(*state);
  return 0;
}

int fixture_setup_unlinked_utxo(void **state) {
  UTXO *utxo;

  utxo = malloc(sizeof(UTXO));
  utxo->amt = 100;
  memset(utxo->public_key_hash, 0xAA, PUB_KEY_HASH_LEN);

  *state = utxo;

  return 0;
}

int fixture_teardown_unlinked_utxo(void **state) {
  free(*state);
  return 0;
}
