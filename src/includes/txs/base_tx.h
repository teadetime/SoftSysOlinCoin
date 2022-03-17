#pragma once

#include "constants.h"
#include "mbedtls/ecdsa.h"

typedef struct Output{
  unsigned long amt;
  unsigned char public_key_hash[PUB_KEY_HASH_LEN];
} Output;

typedef struct Input{
  mbedtls_ecp_point *pub_key;
  size_t sig_len;
  unsigned char signature[SIGNATURE_LEN];
  unsigned char prev_tx_id[TX_HASH_LEN];
  unsigned int prev_utxo_output;
} Input; 

typedef struct Transaction{
  unsigned int num_inputs;
  unsigned int num_outputs;
  Input *inputs;
  Output *outputs;
} Transaction;

typedef struct UTXO{
  unsigned long amt;
  unsigned char public_key_hash[PUB_KEY_HASH_LEN];
  short spent;
} UTXO;

unsigned char *ser_utxo(UTXO *utxo);
UTXO *dser_utxo(unsigned char *data);

/**
 * @brief Get size required to serialize an input
 *  removes the pointer and adds the length of data pointed to
 * 
 * @return int 
 */
int size_input();

unsigned char *ser_input(unsigned char *dest, Input *input);
unsigned char *deser_input(Input *dest, unsigned char *src);

/*
Return Size of a transaction, used for serialization and memory allocation
*/
int size_tx(Transaction *tx);

unsigned char *ser_tx(unsigned char *dest, Transaction *tx);
unsigned char *ser_tx_alloc(Transaction *tx);
Transaction* deser_tx(unsigned char *data);

/* Hashes passed transaction
 *
 * dest: Buffer to write hash to, expected to be of size TX_HASH_LEN
 * tx: Transaction to hash
 */
void hash_tx(unsigned char *dest, Transaction *tx);

void print_input(Input *input, char *prefix);
void print_output(Output *output, char *prefix);
void print_tx(Transaction *tx, char *prefix);
