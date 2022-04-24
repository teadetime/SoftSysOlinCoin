#include "fixtures_wallet.h"
#include "crypto.h"
#include "wallet_pool.h"

#include <stdlib.h>
#include <string.h>

int fixture_setup_unlinked_wallet_entry(void **state) {
  WalletEntry *content;

  content = malloc(sizeof(WalletEntry));
  content->amt = 90;
  content->key_pair = gen_keys();
  content->spent = 1;

  *state = content;

  return 0;
}

int fixture_teardown_unlinked_wallet_entry(void **state) {
  WalletEntry *content;
  content = *state;
  mbedtls_ecp_keypair_free(content->key_pair);
  free(content);
  return 0;
}

int fixture_setup_unlinked_keypair(void **state) {
  *state = gen_keys();
  return 0;
}

int fixture_teardown_unlinked_keypair(void **state) {
  mbedtls_ecp_keypair_free(*state);
  return 0;
}
