#pragma once

#include "wallet_pool.h"
#include "ser_key.h"

#define WALLET_ENTRY_SER_LEN \
  KEYPAIR_SER_LEN + \
  sizeof(((WalletEntry*)0)->amt) + \
  sizeof(((WalletEntry*)0)->spent)

/******************************************************************************
 * Wallet Entries
 ******************************************************************************/

/**
 * @brief Serialize a WalletEntry
 *
 * @param dest Buffer of length size_ser_wallet_entry() to write to
 * @param wallet_entry WalletEntry to serialize
 * @return Number of bytes written if succesfull, -1 otherwise
 */
ssize_t ser_wallet_entry(unsigned char *dest, WalletEntry *wallet_entry);

/**
 * @brief Allocates memory and serializes a WalletEntry
 *
 * @param written Stores number of bytes written if succesfull, -1 otherwise
 * @param wallet_entry WalletEntry to serialize
 * @return Serialized WalletEntry of length size_ser_wallet_entry() if
 *   succesfull, NULL otherwise
 */
unsigned char *ser_wallet_entry_alloc(ssize_t *written, WalletEntry *wallet_entry);

/**
 * @brief Deserialize a WalletEntry
 *
 * @param dest WalletEntry to write to
 * @param src Buffer of length size_ser_wallet_entry() to read from
 * @return Number of bytes read if succesfull, -1 otherwise
 */
ssize_t deser_wallet_entry(WalletEntry *dest, unsigned char *src);

/**
 * @brief Allocate memory and deserialize a WalletEntry
 *
 * @param read Stores number of bytes read if succesfull, -1 otherwise
 * @param src Buffer of length size_ser_wallet_entry() to read from
 * @return Deserialized WalletEntry if succesfull, NULL otherwise
 */
WalletEntry *deser_wallet_entry_alloc(ssize_t* read, unsigned char *src);
