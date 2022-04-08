#pragma once
#include "base_tx.h"

/**
 * @brief Serializes a UTXO
 * 
 * @param utxo UTXO to serialize
 * @return unsigned char* serialized UTXO
 */
unsigned char *ser_utxo(UTXO *utxo);

/**
 * @brief Deserializes a UTXO
 * 
 * @param data Serialized UTXO
 * @return UTXO* Deserialized UTO
 */
UTXO *dser_utxo(unsigned char *data);

/**
 * @brief Serialize an input
 * 
 * @param dest buffer for serialization output of size size_input()
 * @param input input to desrialized
 * @return unsigned char* next byte after serialization
 */
unsigned char *ser_input(unsigned char *dest, Input *input);

/**
 * @brief Deserialize an input
 * 
 * @param dest Input deserialization destination
 * @param src buffer containing serialized input
 * @return unsigned char* next byte of data in src buffer
 */
unsigned char *deser_input(Input *dest, unsigned char *src);

/**
 * @brief Serializes an entire TX
 * 
 * @param dest destination buffer for serialized TX, size of size_tx(a_tx)
 * @param tx Transaction to serialize
 * @return unsigned char* byte after serialization in dest
 */
unsigned char *ser_tx(unsigned char *dest, Transaction *tx);

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
Transaction* deser_tx(unsigned char *data);
