#include "handle_tx.h"
#include "base_tx.h"
#include "mempool.h"
#include "utxo_to_tx.h"

int handle_tx(Transaction *tx){
  if(mempool_add(tx) == NULL){
    return 1;
  }
  // NOTE: UTXO_to_tx_add_tx exits on duplicate data so checking for zero is irrelevant
  if(utxo_to_tx_add_tx(tx) != 0){
    return 2;
  }
  return 0;
}