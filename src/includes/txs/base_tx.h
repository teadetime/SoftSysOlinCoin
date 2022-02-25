#pragma once

#include "constants.h"

#define LOCK_SCRIPT_LEN 48
#define SIGNATURE_LEN 48

typedef struct Output{
  unsigned long amt;
  unsigned char public_key_hash[LOCK_SCRIPT_LEN];
} Output;

typedef struct Input{
  char signature[SIGNATURE_LEN];
  unsigned char prev_tx_id[TX_HASH_LEN];
  int prev_utxo_output;
} Input; 

typedef struct Transaction{
  int num_inputs;
  int num_outputs;
  Input *inputs;
  Output *outputs;
} Transaction;

typedef struct UTXO{
  unsigned long amt;
  char signature[SIGNATURE_LEN];
  short spent;
} UTXO;

char *ser_utxo(UTXO *utxo);
UTXO *dser_utxo(char *data);

/*
Return Size of a transaction, used for serialization and memory allocation
*/
int size_tx(Transaction *tx);

char *ser_tx(char *dest, Transaction *tx);
char *ser_tx_alloc(Transaction *tx);
Transaction* deser_tx(char *data);
void hash_tx(Transaction *tx, unsigned char *buf);

void print_input(Input *input);
void print_output(Output *output);
void print_tx(Transaction *tx);
