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
/**
 * @brief Create a copy of a transaction with blank signatures and hash it for 
 * signature validation
 * 
 * @param blank_hash destination for the blank signature hash
 * @param tx transaction to blank and hash
 */
void create_blank_sig_txhash(unsigned char *blank_hash, Transaction *tx);

/**
 * @brief Check if a given input is unlockable iwth it's unlock script
 * 
 * @param input input to check
 * @param blank_tx_hash blank tx hash that the tx was signed with
 * @return int 0 if unlockable not zero if unusable
 */
int check_input_unlockable(Input *input, unsigned char *blank_tx_hash);

/**
 * @brief Check if the inputs to a transaction exists in the utxo pool ignoring 
 * spent flag now. This cna likely be replaced by just checkign to see if the 
 * tx is in the mempool
 * 
 * @param input input to validate
 * @return int 0 if input referneces a valid utxo, not zero if not
 */
int validate_input_matches_utxopool(Input *input);

/**
 * @brief Wrapper to validate a transactions input in various ways, 
 * inputs size, utxo available, unlockable
 * 
 * @param input input to validate
 * @param blank_tx_hash tx hash of the larger transaction with signatures blanked
 * @return int 0 if valid input, not zero if invalid input
 */
int validate_input(Input *input, unsigned char *blank_tx_hash);

/**
 * @brief Check if a Transaction (Not a coinbase tx) is malformed 
 * this should be the first check run on a transaction
 * 
 * @param tx transaction to validate
 * @return int 0 if transaction is valid, not zero if nor
 */
int validate_tx_parts_not_null(Transaction *tx);

/**
 * @brief Check if a coinbase TX has a Null value in an improper place
 * 
 * @param tx coinbase tx to check 
 * @return int 0 if a valid transaction, not zero if invalid
 */
int validate_coinbase_tx_parts_not_null(Transaction *tx);
  
/**
 * @brief Overall wrapper to validate an entire transaction that is NOT a Coinbase_tx
 * 
 * @param tx transaction to validate
 * @return int 0 if transaction is valid, not zero if invalid
 */
int validate_tx_shared(Transaction *tx);

/**
 * @brief Check if UTXO is double-spent in a transaction
 * 
 * @param tx transaction to check
 * @return int 0 if valid, not zero if invalid
 */
int check_tx_double_spent(Transaction *tx);

/**
 * @brief Check if an UTXO in a transaction is already referenced in the mempool
 * 
 * @param tx transaction to check
 * @return int 0 if valid, not zero if conflict found
 */
int check_inputs_not_in_mempool(Transaction *tx);

/**
 * @brief Validate incoming transactions from the network (different than 
 * block transaction validation)
 * 
 * @param tx transaction to validate
 * @return int 0 if incoming tx is valid, not zero if invalid
 */
int validate_tx_incoming(Transaction *tx);