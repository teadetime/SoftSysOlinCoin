#pragma once

#include "constants.h"

#define LOCK_SCRIPT_LEN 48
#define SIGNATURE_LEN 48

typedef struct Output{
  unsigned long amt;
  unsigned char public_key_hash[LOCK_SCRIPT_LEN];
} Output;

typedef struct Input{
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
  unsigned char public_key_hash[LOCK_SCRIPT_LEN];
  short spent;
} UTXO;

unsigned char *ser_utxo(UTXO *utxo);
UTXO *dser_utxo(unsigned char *data);

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

void print_input(Input *input);
void print_output(Output *output);
void print_tx(Transaction *tx);
