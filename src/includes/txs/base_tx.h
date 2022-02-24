#ifndef BASE_TX_H
#define BASE_TX_H

#include "constants.h"

#define LOCK_SCRIPT_LEN 48
#define SIGNATURE_LEN 48

typedef struct Output{
    unsigned long amt;
    char public_key_hash[LOCK_SCRIPT_LEN];
} Output;

typedef struct Input{
    char signature[SIGNATURE_LEN];
    char prev_tx_id[TX_HASH_LEN];
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

char * ser_UTXO(UTXO *utxo);
UTXO * dser_UTXO(char * data);
char * ser_tx(Transaction *tx);
Transaction* deser_tx(char *data);

void print_input(Input *input);
void print_output(Output *output);
void print_tx(Transaction *tx);

#endif
