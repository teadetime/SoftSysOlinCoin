#include "handle_tx.h"
#include "base_tx.h"
#include "mempool.h"
#include "utxo_to_tx.h"

int handle_tx(Transaction *tx){
  Transaction *coppied_tx = copy_tx(tx);
  if(mempool_add(coppied_tx) == NULL){
    return 1;
  }
  return 0;
}