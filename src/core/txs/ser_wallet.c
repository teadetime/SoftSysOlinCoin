#include "ser_wallet.h"

#define RETURN_SER(data, ret, ptr) \
  { \
    if (ptr != NULL) \
      *ptr = ret; \
    if (ret == -1) { \
      free(data); \
      return NULL; \
    } \
    return data; \
  }

/******************************************************************************
 * Wallet Entries
 ******************************************************************************/

ssize_t ser_wallet_entry(unsigned char *dest, WalletEntry *wallet_entry) {
  unsigned char *amt = dest + ser_keypair(dest, wallet_entry->key_pair);

  memcpy(amt, &(wallet_entry->amt), sizeof(wallet_entry->amt));
  unsigned char *spent = amt + sizeof(wallet_entry->amt);

  memcpy(spent, &(wallet_entry->spent), sizeof(wallet_entry->spent));
  unsigned char *end = spent + sizeof(wallet_entry->spent);

  return end - dest;
}

unsigned char *ser_wallet_entry_alloc(ssize_t *written, WalletEntry *wallet_entry) {
  unsigned char *data;
  ssize_t ret;
  data = malloc(WALLET_ENTRY_SER_LEN);
  ret = ser_wallet_entry(data, wallet_entry);
  RETURN_SER(data, ret, written)
}

ssize_t deser_wallet_entry(WalletEntry *dest, unsigned char *src) {
  ssize_t ser_key_len;

  dest->key_pair = deser_keypair_alloc(&ser_key_len, src);
  unsigned char *amt = src + ser_key_len;

  memcpy(&(dest->amt), amt, sizeof(((WalletEntry*)0)->amt));
  unsigned char *spent = amt + sizeof(((WalletEntry*)0)->amt);

  memcpy(&(dest->spent), spent, sizeof(((WalletEntry*)0)->spent));
  unsigned char *end = spent + sizeof(((WalletEntry*)0)->spent);

  return end - src;
}

WalletEntry *deser_wallet_entry_alloc(ssize_t* read, unsigned char *src) {
  WalletEntry *wallet_entry;
  ssize_t ret;
  wallet_entry = malloc(sizeof(WalletEntry));
  ret = deser_wallet_entry(wallet_entry, src);
  RETURN_SER(wallet_entry, ret, read)
}
