#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <setjmp.h>
#include <cmocka.h>

#include "constants.h"
#include "crypto.h"

static void test_hash_consistency(void **state) {
  unsigned char dest[TX_HASH_LEN];
  unsigned char dest2[TX_HASH_LEN];
  unsigned char hash_buf[] = "THIS IS A TEST";
  (void)state;
  hash_sha256(dest, hash_buf, sizeof(hash_buf));
  hash_sha256(dest2, hash_buf, sizeof(hash_buf));
  assert_memory_equal(dest, dest2, TX_HASH_LEN);
}

static void test_hash_consistency_long(void **state) {
  unsigned char dest[TX_HASH_LEN];
  unsigned char dest2[TX_HASH_LEN];
  unsigned char hash_buf_long[] = "THdsaflkjkljjklsadflkasdfkjlasdfasdf32145234 b5234 3455432 3245";
  (void)state;
  hash_sha256(dest, hash_buf_long, sizeof(hash_buf_long));
  hash_sha256(dest2, hash_buf_long, sizeof(hash_buf_long));
  assert_memory_equal(dest, dest2, TX_HASH_LEN);
}

static void test_hash_different(void **state) {
  unsigned char dest[TX_HASH_LEN];
  unsigned char dest2[TX_HASH_LEN];
  unsigned char hash_buf_a[] = "test1";
  unsigned char hash_buf_b[] = "test2";
  (void)state;
  hash_sha256(dest, hash_buf_a, sizeof(hash_buf_a));
  hash_sha256(dest2, hash_buf_b, sizeof(hash_buf_b));
  assert_memory_not_equal(dest, dest2, TX_HASH_LEN);
}

int main() {
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_hash_consistency),
    cmocka_unit_test(test_hash_consistency_long),
    cmocka_unit_test(test_hash_different),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
