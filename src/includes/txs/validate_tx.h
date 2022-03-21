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
void create_blank_sig_txhash(unsigned char *blank_hash, Transaction *tx);

int check_input_unlockable(Transaction *tx);