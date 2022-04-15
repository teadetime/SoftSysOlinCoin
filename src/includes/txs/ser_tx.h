#pragma once
#include "base_tx.h"

size_t size_ser_utxo();

/**
 * @brief Serializes a UTXO
 * 
 * @param utxo UTXO to serialize
 * @return unsigned char* serialized UTXO
 */
ssize_t ser_utxo(unsigned char *dest, UTXO *utxo);
unsigned char *ser_utxo_alloc(UTXO *utxo);

/**
 * @brief Deserializes a UTXO
 * 
 * @param src Serialized UTXO
 * @return UTXO* Deserialized UTO
 */
ssize_t deser_utxo(UTXO *dest, unsigned char *src);
UTXO *deser_utxo_alloc(unsigned char *src);

/**
 * @brief Get size required to serialize an input
 *  removes the pointer and adds the length of data pointed to
 * 
 * @return int 
 */
size_t size_ser_input();

/**
 * @brief Serialize an input
 * 
 * @param dest buffer for serialization output of size size_input()
 * @param input input to desrialized
 * @return unsigned char* next byte after serialization
 */
ssize_t ser_input(unsigned char *dest, Input *input);
unsigned char *ser_input_alloc(Input *input);

/**
 * @brief Deserialize an input
 * 
 * @param dest Input deserialization destination
 * @param src buffer containing serialized input
 * @return unsigned char* next byte of data in src buffer
 */
ssize_t deser_input(Input *dest, unsigned char *src);
Input *deser_input_alloc(unsigned char *src);

/*
Return Size of a transaction, used for serialization and memory allocation
*/
size_t size_ser_tx(Transaction *tx);

/**
 * @brief Serializes an entire TX
 * 
 * @param dest destination buffer for serialized TX, size of size_tx(a_tx)
 * @param tx Transaction to serialize
 * @return unsigned char* byte after serialization in dest
 */
ssize_t ser_tx(unsigned char *dest, Transaction *tx);

/**
 * @brief Serializes a TX and allocates memory for it
 * 
 * @param tx Transaction to serialize 
 * @return unsigned char* buffer of length size_tx(tx) containing serialized tx
 */
unsigned char *ser_tx_alloc(Transaction *tx);

/**
 * @brief Deserialize TX
 * 
 * @param data Buffer containing serialized Transation
 * @return Transaction* Transaction deserialized
 */
ssize_t deser_tx(Transaction *dest, unsigned char *data);
Transaction* deser_tx_alloc(unsigned char *data);
