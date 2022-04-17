#include <stdlib.h>
#include <string.h>

#include "ser_tx.h"
#include "ser_key.h"
#include "base_tx.h"
#include "crypto.h"

#define RETURN_SER(data, ret, ptr) \
  { \
    if (ptr != NULL) \
      *ptr = ret; \
    if (ret == -1) { \
      free(data); \
      return NULL; \
    } \
    return data; \
  }

/******************************************************************************
 * UTXOs
 ******************************************************************************/

size_t size_ser_utxo() {
  return sizeof(((UTXO*)0)->amt) + sizeof(((UTXO*)0)->public_key_hash);
}

ssize_t ser_utxo(unsigned char *dest, UTXO *utxo) {
  memcpy(dest, &(utxo->amt), sizeof(utxo->amt));

  unsigned char *sig = dest + sizeof(utxo->amt);
  memcpy(sig, &(utxo->public_key_hash), sizeof(utxo->public_key_hash));

  return (sig + sizeof(utxo->public_key_hash)) - dest;
}

unsigned char *ser_utxo_alloc(ssize_t *written, UTXO *utxo) {
  unsigned char *data;
  ssize_t ret;
  data = malloc(size_ser_utxo());
  ret = ser_utxo(data, utxo);
  RETURN_SER(data, ret, written)
}

ssize_t deser_utxo(UTXO *dest, unsigned char *src) {
  memcpy(&(dest->amt), src, sizeof(((UTXO*)0)->amt));

  unsigned char *sig = src + sizeof(((UTXO*)0)->amt);
  memcpy(&(dest->public_key_hash), sig, sizeof(((UTXO*)0)->public_key_hash));

  return (sig + sizeof(((UTXO*)0)->public_key_hash)) - src;
}

UTXO *deser_utxo_alloc(ssize_t *read, unsigned char *src) {
  UTXO *utxo;
  ssize_t ret;
  utxo = malloc(sizeof(UTXO));
  ret = deser_utxo(utxo, src);
  RETURN_SER(utxo, ret, read)
}

/******************************************************************************
 * Inputs
 ******************************************************************************/

size_t size_ser_input() {
  return PUB_KEY_SER_LEN + sizeof(((Input*)0)->sig_len) + SIGNATURE_LEN +
    TX_HASH_LEN + sizeof(((Input*)0)->prev_utxo_output);
}

ssize_t ser_input(unsigned char *dest, Input *input) {
  unsigned char *sig_len = dest + ser_pub_key(dest, input->pub_key);

  memcpy(sig_len, &(input->sig_len), sizeof(input->sig_len));

  unsigned char *sig = sig_len + sizeof(input->sig_len);
  memcpy(sig, input->signature, SIGNATURE_LEN);

  unsigned char *prev_tx = sig + SIGNATURE_LEN;
  memcpy(prev_tx, input->prev_tx_id, TX_HASH_LEN);

  unsigned char *vout = prev_tx + TX_HASH_LEN;
  memcpy(vout, &(input->prev_utxo_output), sizeof(input->prev_utxo_output));

  return (vout + sizeof(input->prev_utxo_output)) - dest;
}

unsigned char *ser_input_alloc(ssize_t *written, Input *input) {
  unsigned char *data;
  ssize_t ret;
  data = malloc(size_ser_input());
  ret = ser_input(data, input);
  RETURN_SER(data, ret, written)
}

ssize_t deser_input(Input *dest, unsigned char *src) {
  dest->pub_key = malloc(sizeof(mbedtls_ecp_point));
  mbedtls_ecp_point_init(dest->pub_key);
  deser_pub_key(dest->pub_key, src);

  unsigned char *sig_len = src + PUB_KEY_SER_LEN;
  memcpy(&(dest->sig_len), sig_len, sizeof(((Input*)0)->sig_len));

  unsigned char *sig = sig_len + sizeof(((Input*)0)->sig_len);
  memcpy(dest->signature, sig, SIGNATURE_LEN);

  unsigned char *prev_tx = sig + SIGNATURE_LEN;
  memcpy(dest->prev_tx_id, prev_tx, TX_HASH_LEN);

  unsigned char *vout = prev_tx + TX_HASH_LEN;
  memcpy(&(dest->prev_utxo_output), vout, sizeof(((Input*)0)->prev_utxo_output));

  return (vout + sizeof(((Input*)0)->prev_utxo_output)) - src;
}

Input *deser_input_alloc(ssize_t *read, unsigned char *src) {
  Input *input;
  ssize_t ret;
  input = malloc(sizeof(Input));
  ret = deser_input(input, src);
  RETURN_SER(input, ret, read)
}

/******************************************************************************
 * Transactions
 ******************************************************************************/

size_t size_ser_tx(Transaction *tx) {
  return (sizeof(tx->num_inputs) + sizeof(tx->num_outputs) +
    tx->num_inputs * size_ser_input() + tx->num_outputs * sizeof(Output));
}

ssize_t ser_tx(unsigned char *dest, Transaction *tx) {
  memcpy(dest, &(tx->num_inputs), sizeof(tx->num_inputs));

  unsigned char *num_outputs = dest + sizeof(tx->num_inputs);
  memcpy(num_outputs, &(tx->num_outputs), sizeof(tx->num_outputs));

  unsigned char *inputs = num_outputs + sizeof(tx->num_outputs);
  for (size_t i = 0; i < tx->num_inputs; i++)
    inputs += ser_input(inputs, &(tx->inputs[i]));

  unsigned char *outputs = inputs;
  memset(outputs, 0, tx->num_outputs * sizeof(Output));
  memcpy(outputs, tx->outputs, tx->num_outputs * sizeof(Output));

  unsigned char *end = outputs + tx->num_outputs * sizeof(Output);
  return end - dest;
}

unsigned char *ser_tx_alloc(ssize_t *written, Transaction *tx) {
  unsigned char *data;
  ssize_t ret;
  data = malloc(size_ser_tx(tx));
  ret = ser_tx(data, tx);
  RETURN_SER(data, ret, written)
}

ssize_t deser_tx(Transaction *dest, unsigned char *src) {
  memcpy(&(dest->num_inputs), src, sizeof(((Transaction*)0)->num_inputs));

  unsigned char *num_outputs = src + sizeof(((Transaction*)0)->num_inputs);
  memcpy(&(dest->num_outputs), num_outputs, sizeof(((Transaction*)0)->num_outputs));

  unsigned char *inputs = num_outputs + sizeof(((Transaction*)0)->num_outputs);
  dest->inputs = malloc(dest->num_inputs * sizeof(Input));
  for (size_t i = 0; i < dest->num_inputs; i++)
    inputs += deser_input(dest->inputs + i, inputs);

  unsigned char *outputs = inputs;
  unsigned int output_sz = dest->num_outputs * sizeof(Output);
  dest->outputs = malloc(output_sz);
  memcpy(dest->outputs, outputs, output_sz);

  return (outputs + output_sz) - src;
}

Transaction *deser_tx_alloc(ssize_t *read, unsigned char *src) {
  Transaction *tx;
  ssize_t ret;
  tx = malloc(sizeof(Transaction));
  ret = deser_tx(tx, src);
  RETURN_SER(tx, ret, read)
}
