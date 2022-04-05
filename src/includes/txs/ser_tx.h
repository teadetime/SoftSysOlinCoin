#pragma once
#include "base_tx.h"

unsigned char *ser_utxo(UTXO *utxo);
UTXO *dser_utxo(unsigned char *data);

unsigned char *ser_input(unsigned char *dest, Input *input);
unsigned char *deser_input(Input *dest, unsigned char *src);

unsigned char *ser_tx(unsigned char *dest, Transaction *tx);
unsigned char *ser_tx_alloc(Transaction *tx);
Transaction* deser_tx(unsigned char *data);
