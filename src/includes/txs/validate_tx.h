/**
 * @file validate_tx.h
 * @author Nathan Faber nfaber@olin.edu
 * @brief 
 * @version 0.1
 * @date 2022-03-20
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "base_tx.h"
int check_inputs_unlockable(Transaction *tx);

int validate_tx(Transaction *tx);