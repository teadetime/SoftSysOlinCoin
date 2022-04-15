#pragma once
#include <stdlib.h>
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
} UTXO;

/**
 * @brief Get size required to serialize an input
 *  removes the pointer and adds the length of data pointed to
 * 
 * @return int 
 */
int size_input();

/*
Return Size of a transaction, used for serialization and memory allocation
*/
int size_tx(Transaction *tx);

/* Hashes passed transaction
 *
 * dest: Buffer to write hash to, expected to be of size TX_HASH_LEN
 * tx: Transaction to hash
 */
void hash_tx(unsigned char *dest, Transaction *tx);

/**
 * @brief Free transaction
 *
 * @param tx Transaction to free
 */
void free_tx(Transaction *tx);

/**
 * @brief Copy transaction
 *
 * Copies transaction, returning NULL if the passed transaction is NULL
 *
 * @param tx Transaction to copy
 * @return Copy of passed transcation
 */
Transaction *copy_tx(Transaction *tx);

void print_input(Input *input, char *prefix);
void print_output(Output *output, char *prefix);
void print_tx(Transaction *tx, char *prefix);

void pretty_print_tx(Transaction *tx, char *prefix);
