#pragma once

#include "base_tx.h"

#include "constants.h"
#include "ser_key.h"

/******************************************************************************
 * UTXOs
 ******************************************************************************/

#define UTXO_SER_LEN \
  ( \
    sizeof(((UTXO*)0)->amt) + \
    sizeof(((UTXO*)0)->public_key_hash) \
  )

/**
 * @brief Serialize a UTXO
 *
 * @param dest Buffer of length size_ser_utxo() to write to
 * @param utxo UTXO to serialize
 * @return Number of bytes written if succesfull, -1 otherwise
 */
ssize_t ser_utxo(unsigned char *dest, UTXO *utxo);

/**
 * @brief Allocates memory and serializes a UTXO
 *
 * @param written Stores number of bytes written if succesfull, -1 otherwise
 * @param utxo UTXO to serialize
 * @return Serialized UTXO of length size_ser_utxo() if succesfull, NULL
 *   otherwise
 */
unsigned char *ser_utxo_alloc(ssize_t *written, UTXO *utxo);

/**
 * @brief Deserialize a UTXO
 *
 * @param dest UTXO to write to
 * @param src Buffer of length size_ser_utxo() to read from
 * @return Number of bytes read if succesfull, -1 otherwise
 */
ssize_t deser_utxo(UTXO *dest, unsigned char *src);

/**
 * @brief Allocate memory and deserialize a UTXO
 *
 * @param read Stores number of bytes read if succesfull, -1 otherwise
 * @param src Buffer of length size_ser_utxo() to read from
 * @return Deserialized UTXO if succesfull, NULL otherwise
 */
UTXO *deser_utxo_alloc(ssize_t* read, unsigned char *src);

/******************************************************************************
 * Inputs
 ******************************************************************************/

#define INPUT_SER_LEN \
  ( \
    PUB_KEY_SER_LEN + \
    sizeof(((Input*)0)->sig_len) + \
    SIGNATURE_LEN + \
    TX_HASH_LEN + \
    sizeof(((Input*)0)->prev_utxo_output) \
  )

/**
 * @brief Serialize an Input
 *
 * @param dest Buffer of length size_ser_input() to write to
 * @param input Input to serialize
 * @return Number of bytes written if succesfull, -1 otherwise
 */
ssize_t ser_input(unsigned char *dest, Input *input);

/**
 * @brief Allocates memory and serializes an Input
 *
 * @param written Stores number of bytes written if succesfull, -1 otherwise
 * @param input Input to serialize
 * @return Serialized input of length size_ser_input() if succesfull, NULL
 *   otherwise
 */
unsigned char *ser_input_alloc(ssize_t *written, Input *input);

/**
 * @brief Deserialize an Input
 *
 * @param dest Input to write to
 * @param src Buffer of length size_ser_input() to read from
 * @return Number of bytes read if succesfull, -1 otherwise
 */
ssize_t deser_input(Input *dest, unsigned char *src);

/**
 * @brief Allocate memory and deserialize an Input
 *
 * @param read Stores number of bytes read if succesfull, -1 otherwise
 * @param src Buffer of length size_ser_input() to read from
 * @return Deserialized input if succesfull, NULL otherwise
 */
Input *deser_input_alloc(ssize_t *read, unsigned char *src);

/******************************************************************************
 * Transactions
 ******************************************************************************/

/**
 * @brief Get size of a Transaction if serialized
 *
 * @param tx Transaction to get size of
 * @return Size of tx if serialized
 */
size_t size_ser_tx(Transaction *tx);

/**
 * @brief Serialize a Transaction
 *
 * @param dest Buffer of length size_ser_tx(tx) to write to
 * @param tx Transaction to serialize
 * @return Number of bytes written if succesfull, -1 otherwise
 */
ssize_t ser_tx(unsigned char *dest, Transaction *tx);

/**
 * @brief Allocates memory and serializes a Transaction
 *
 * @param written Stores number of bytes written if succesfull, -1 otherwise
 * @param tx Transaction to serialize
 * @return unsigned char* Serialized tx of length size_ser_tx(tx) if succesfull,
 *   NULL otherwise
 */
unsigned char *ser_tx_alloc(ssize_t *written, Transaction *tx);

/**
 * @brief Deserialize a Transaction
 *
 * @param dest Transaction to write to
 * @param src Buffer of length size_ser_tx(tx) to read from
 * @return Number of bytes read if succesfull, -1 otherwise
 */
ssize_t deser_tx(Transaction *dest, unsigned char *src);

/**
 * @brief Allocate memory and deserialize a Transaction
 *
 * @param read Stores number of bytes read if succesfull, -1 otherwise
 * @param src Buffer of length size_ser_tx(tx) to read from
 * @return Deserialized tx if succesfull, NULL otherwise
 */
Transaction* deser_tx_alloc(ssize_t *read, unsigned char *src);
