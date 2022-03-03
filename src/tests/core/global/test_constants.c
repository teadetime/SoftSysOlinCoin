#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "minunit.h"
#include "constants.h"
#include "base_tx.h"

int tests_run = 0;

static char *test_hash_consistency() {
  unsigned char *dest = malloc(TX_HASH_LEN);
  unsigned char *dest2 = malloc(TX_HASH_LEN);
  unsigned char hash_buf[] = "THIS IS A TEST";
  hash_sha256(dest, hash_buf, sizeof(hash_buf));
  hash_sha256(dest2, hash_buf, sizeof(hash_buf));
  mu_assert(
    "Hashing isn't consistent",
    memcmp(dest,dest2 , TX_HASH_LEN) == 0
  );
  return NULL;
}
static char *test_hash_consistency_long() {
  unsigned char *dest = malloc(TX_HASH_LEN);
  unsigned char *dest2 = malloc(TX_HASH_LEN);
  unsigned char hash_buf_long[] = "THdsaflkjkljjklsadflkasdfkjlasdfasdf32145234 b5234 3455432 3245";
  hash_sha256(dest, hash_buf_long, sizeof(hash_buf_long));
  hash_sha256(dest2, hash_buf_long, sizeof(hash_buf_long));
  mu_assert(
    "Hashing isn't consistent with long",
    memcmp(dest,dest2 , TX_HASH_LEN) == 0
  );
  return NULL;
}
static char *all_tests() {
  mu_run_test(test_hash_consistency);
  mu_run_test(test_hash_consistency_long);
  return NULL;
}

int main() {
  char *result = all_tests();
  if (result != NULL) {
    printf("%s\n", result);
  } else {
    printf("constants.c passing!\n");
  }
  printf("Tests run: %d\n", tests_run);

  return result != 0;
}
