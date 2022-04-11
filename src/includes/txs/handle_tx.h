#include "base_tx.h"
/**
 * @brief Do corresponding updates for a validated transaction 
 * (Mempool and utxo->mempool mapping)
 * 
 * @param tx Transaction to handle
 * @return int 0 if success, not zero otherwise
 */
int handle_tx(Transaction *tx);