#include "handle_tx.h"
#include "base_tx.h"
#include "mempool.h"
#include "utxo_to_tx.h"

int handle_tx(Transaction *tx){
  if(mempool_add(tx) == NULL){
    return 1;
  }
  return 0;
}