#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <setjmp.h>
#include <cmocka.h>

#include "base_tx.h"
#include "fixtures_tx.h"

static void test_copy_tx(void **state) {
  Transaction *tx, *copy;
  unsigned char tx_hash[TX_HASH_LEN], copy_hash[TX_HASH_LEN];

  tx = *state;
  hash_tx(tx_hash, tx);
  copy = copy_tx(tx);
  hash_tx(copy_hash, copy);

  assert_memory_equal(tx_hash, copy_hash, TX_HASH_LEN);
}

int main() {
  const struct CMUnitTest tests[] = {
    cmocka_unit_test_setup_teardown(
      test_copy_tx,
      fixture_setup_unlinked_tx,
      fixture_teardown_unlinked_tx
    )
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
