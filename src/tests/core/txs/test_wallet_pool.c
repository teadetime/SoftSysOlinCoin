#include <stdio.h>
#include "minunit.h"
#include "wallet.h"

int tests_run = 0;

static char *test_wallet_pool_init() {
  wallet_pool_init();
  mu_assert(
      "Wallet pool was not initialized",
      verified_wallet_pool.data != NULL
  );
  mu_assert(
      "Wallet pool length incorrect",
      verified_wallet_pool.len == 0
  );
  mu_assert(
      "Wallet pool total length incorrect",
      verified_wallet_pool.total_len == 1
  );
  return NULL;
}

static char *test_wallet_pool_append() {
  WalletEntry *entry;

  wallet_pool_init();

  entry = malloc(sizeof(WalletEntry));
  memset(entry, 0, sizeof(WalletEntry));
  entry->amt = 100;
  wallet_pool_append(entry);

  mu_assert(
      "Wallet pool entry not appended",
      memcmp(verified_wallet_pool.data[0], entry, sizeof(WalletEntry)) == 0
  );
  mu_assert(
      "Wallet pool length post append incorrect",
      verified_wallet_pool.len == 1
  );

  return NULL;
}

static char *test_wallet_pool_pop() {
  WalletEntry *entry, *ret_entry;

  wallet_pool_init();

  entry = malloc(sizeof(WalletEntry));
  memset(entry, 0, sizeof(WalletEntry));
  entry->amt = 100;
  wallet_pool_append(entry);
  ret_entry = wallet_pool_pop();

  mu_assert(
      "Wallet pool entry not popped",
      memcmp(verified_wallet_pool.data[0], ret_entry, sizeof(WalletEntry)) == 0
  );
  mu_assert(
      "Wallet pool length post pop incorrect",
      verified_wallet_pool.len == 0
  );

  return NULL;
}

static char *all_tests() {
  mu_run_test(test_wallet_pool_init);
  mu_run_test(test_wallet_pool_append);
  mu_run_test(test_wallet_pool_pop);
  return NULL;
}

int main() {
  char *result = all_tests();
  if (result != NULL) {
    printf("%s\n", result);
  } else {
    printf("wallet.c wallet pool passing!\n");
  }
  printf("Tests run: %d\n", tests_run);

  return result != 0;
}
