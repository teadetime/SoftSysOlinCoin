#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <setjmp.h>
#include <cmocka.h>

#include "ser_wallet.h"
#include "fixtures_wallet.h"

static void test_ser_wallet(void **state) {
  WalletEntry *entry, *desered_entry;
  unsigned char *sered_entry;
  ssize_t read_ser_entry, written_ser_entry;

  entry = *state;
  sered_entry = ser_wallet_entry_alloc(&read_ser_entry, entry);
  desered_entry = deser_wallet_entry_alloc(&written_ser_entry, sered_entry);

  assert_int_equal(read_ser_entry, WALLET_ENTRY_SER_LEN);
  assert_int_equal(read_ser_entry, written_ser_entry);

  assert_int_equal(entry->amt, desered_entry->amt);
  assert_int_equal(entry->spent, desered_entry->spent);

  assert_true(mbedtls_ecp_point_cmp(
    &entry->key_pair->private_Q,
    &desered_entry->key_pair->private_Q
  ) == 0);
  assert_true(mbedtls_mpi_cmp_mpi(
    &entry->key_pair->private_d,
    &desered_entry->key_pair->private_d
  ) == 0);
  assert_true(mbedtls_ecp_check_privkey(
    &entry->key_pair->private_grp,
    &desered_entry->key_pair->private_d
  ) == 0);  // Proxy for checking group, breaks if group if wrong

  free(sered_entry);
  free(desered_entry);
}

static void test_ser_wallet_pad(void **state) {
  WalletEntry *entry, *pad_entry;
  unsigned char *sered_entry;
  unsigned char sered_entry_2[WALLET_ENTRY_SER_LEN];
  ssize_t read_ser_entry;

  entry = *state;
  sered_entry = ser_wallet_entry_alloc(&read_ser_entry, entry);
  // Ensure that we are actually filling the serialization buffer
  for (int i = 0; i < 5; i++) {
    memset(sered_entry_2, i, WALLET_ENTRY_SER_LEN);
    ser_wallet_entry(sered_entry_2, entry);
    assert_memory_equal(sered_entry, sered_entry_2, WALLET_ENTRY_SER_LEN);
  }

  // Ensure that we don't have padding bytes
  pad_entry = malloc(sizeof(WalletEntry));
  for (int i = 0; i < 5; i++) {
    memset(pad_entry, i, sizeof(WalletEntry));
    pad_entry->amt = entry->amt;
    pad_entry->spent = entry->spent;
    pad_entry->key_pair = entry->key_pair;
    ser_wallet_entry(sered_entry_2, pad_entry);
    assert_memory_equal(sered_entry, sered_entry_2, WALLET_ENTRY_SER_LEN);
  }

  free(pad_entry);
  free(sered_entry);
}

int main() {
  const struct CMUnitTest tests[] = {
    cmocka_unit_test_setup_teardown(
      test_ser_wallet,
      fixture_setup_unlinked_wallet_entry,
      fixture_teardown_unlinked_wallet_entry
    ),
    cmocka_unit_test_setup_teardown(
      test_ser_wallet_pad,
      fixture_setup_unlinked_wallet_entry,
      fixture_teardown_unlinked_wallet_entry
    )
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
