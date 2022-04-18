#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "minunit.h"
#include "ser_tx.h"
#include "ser_key.h"
#include "ser_wallet.h"
#include "crypto.h"

int tests_run = 0;

WalletEntry *_make_wallet_entry() {
  WalletEntry *content;
  content = malloc(sizeof(WalletEntry));
  content->amt = 90;
  content->key_pair = gen_keys();
  content->spent = 1;
  return content;
}

static char  *test_ser_wallet() {
  WalletEntry *entry, *desered_entry, *pad_entry;
  unsigned char *sered_entry;
  unsigned char sered_entry_2[WALLET_ENTRY_SER_LEN];
  unsigned char ser_pair_1[KEYPAIR_SER_LEN], ser_pair_2[KEYPAIR_SER_LEN];
  ssize_t read_ser_entry, written_ser_entry;

  entry = _make_wallet_entry();
  sered_entry = ser_wallet_entry_alloc(&read_ser_entry, entry);
  desered_entry = deser_wallet_entry_alloc(&written_ser_entry, sered_entry);

  mu_assert(
    "Num of bytes read incorrect",
    read_ser_entry == WALLET_ENTRY_SER_LEN
  );
  mu_assert(
    "Num of bytes read and written don't match up",
    read_ser_entry == written_ser_entry
  );

  mu_assert(
    "Amount isn't consistent after de-serialization",
    entry->amt == desered_entry->amt
  );
  mu_assert(
    "Spent isn't consistent after de-serialization",
    entry->amt == desered_entry->amt
  );
  ser_keypair(ser_pair_1, entry->key_pair);
  ser_keypair(ser_pair_2, desered_entry->key_pair);
  mu_assert(
    "Key pair hash isn't consistent after de-serialization",
    memcmp(ser_pair_1, ser_pair_2, KEYPAIR_SER_LEN) == 0
  );

  // Ensure that we are actually filling the serialization buffer
  for (int i = 0; i < 5; i++) {
    memset(sered_entry_2, i, WALLET_ENTRY_SER_LEN);
    ser_wallet_entry(sered_entry_2, entry);
    mu_assert(
      "Entire serialization buffer is not being overwritten",
      memcmp(sered_entry, sered_entry_2, WALLET_ENTRY_SER_LEN) == 0
    );
  }

  // Ensure that we don't have padding bytes
  pad_entry = malloc(sizeof(WalletEntry));
  for (int i = 0; i < 5; i++) {
    memset(pad_entry, i, sizeof(WalletEntry));
    pad_entry->amt = entry->amt;
    pad_entry->spent = entry->spent;
    pad_entry->key_pair = entry->key_pair;
    ser_wallet_entry(sered_entry_2, entry);
    mu_assert(
      "Padding bytes were copied into serialization",
      memcmp(sered_entry, sered_entry_2, WALLET_ENTRY_SER_LEN) == 0
    );
  }

  free(entry);
  free(pad_entry);
  free(sered_entry);
  free(desered_entry);

  return NULL;
}

static char *all_tests() {
  mu_run_test(test_ser_wallet);
  return NULL;
}

int main() {
    char *result = all_tests();
  if (result != NULL) {
    printf("%s\n", result);
  } else {
    printf("ser_wallet.c passing!\n");
  }
  printf("Tests run: %d\n", tests_run);

  return result != 0;
}
