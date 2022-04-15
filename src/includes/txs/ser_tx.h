#pragma once

#include "base_tx.h"

/******************************************************************************
 * UTXOs
 ******************************************************************************/
/**
 * @brief Gets size of serialized UTXO
 *
 * @return Size of serialized UTXO
 */
size_t size_ser_utxo();

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
 * @param utxo UTXO to serialize
 * @return unsigned char* Serialized UTXO, of length size_ser_utxo()
 */
unsigned char *ser_utxo_alloc(UTXO *utxo);

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
 * @param src Buffer of length size_ser_utxo() to read from
 * @return Deserialized UTXO
 */
UTXO *deser_utxo_alloc(unsigned char *src);

/******************************************************************************
 * Inputs
 ******************************************************************************/
/**
 * @brief Get size of serialized Input
 *
 * @return Size of serialized Input
 */
size_t size_ser_input();

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
 * @param input Input to serialize
 * @return unsigned char* Serialized Input, of length size_ser_input()
 */
unsigned char *ser_input_alloc(Input *input);

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
 * @param src Buffer of length size_ser_input() to read from
 * @return Deserialized Input
 */
Input *deser_input_alloc(unsigned char *src);

/******************************************************************************
 * Transactions
 ******************************************************************************/
/**
 * @brief Get size of a Transaction if serialized
 *
 * @param tx Transaction to get size of
 * @return Size of Transaction if serialized
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
 * @param tx Transaction to serialize
 * @return unsigned char* Serialized Transaction, of length size_ser_tx(tx)
 */
unsigned char *ser_tx_alloc(Transaction *tx);

/**
 * @brief Deserialize a Transaction
 *
 * @param dest Transaction to write to
 * @param src Buffer of length size_ser_tx(tx) to read from
 * @return Number of bytes read if succesfull, -1 otherwise
 */
ssize_t deser_tx(Transaction *dest, unsigned char *data);


/**
 * @brief Allocate memory and deserialize a Transaction
 *
 * @param src Buffer of length size_ser_tx(tx) to read from
 * @return Deserialized Transaction
 */
Transaction* deser_tx_alloc(unsigned char *data);
