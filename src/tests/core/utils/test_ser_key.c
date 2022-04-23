#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "minunit.h"
#include "crypto.h"
#include "ser_key.h"

int tests_run = 0;

static char  *test_ser_pub_key() {
  mbedtls_ecdsa_context *keypair;
  mbedtls_ecp_point *pub_key, *desered_pub_key;
  unsigned char *sered_pub_key;
  unsigned char sered_pub_key_2[PUB_KEY_SER_LEN];
  ssize_t read, written;

  keypair = gen_keys();
  pub_key = &keypair->private_Q;
  sered_pub_key = ser_pub_key_alloc(&read, pub_key);
  desered_pub_key = deser_pub_key_alloc(&written, sered_pub_key);

  mu_assert(
    "Num of bytes read incorrect",
    read == PUB_KEY_SER_LEN
  );
  mu_assert(
    "Num of bytes read and written don't match up",
    read == written
  );

  mu_assert(
    "Pub key isn't consistent after serialization",
    mbedtls_ecp_point_cmp(pub_key, desered_pub_key) == 0
  );

  // Ensure that we are actually filling the serialization buffer
  for (int i = 0; i < 5; i++) {
    memset(sered_pub_key_2, i, PUB_KEY_SER_LEN);
    ser_pub_key(sered_pub_key_2, pub_key);
    mu_assert(
      "Entire serialization buffer is not being overwritten",
      memcmp(sered_pub_key, sered_pub_key_2, PUB_KEY_SER_LEN) == 0
    );
  }

  mbedtls_ecp_keypair_free(keypair);
  free(sered_pub_key);
  free(desered_pub_key);

  return NULL;
}

static char  *test_ser_priv_key() {
  mbedtls_ecdsa_context *keypair;
  mbedtls_mpi *priv_key, *desered_priv_key;
  unsigned char *sered_priv_key;
  unsigned char sered_priv_key_2[PRIV_KEY_SER_LEN];
  ssize_t read, written;

  keypair = gen_keys();
  priv_key = &keypair->private_d;
  sered_priv_key = ser_priv_key_alloc(&read, priv_key);
  desered_priv_key = deser_priv_key_alloc(&written, sered_priv_key);

  mu_assert(
    "Num of bytes read incorrect",
    read == PRIV_KEY_SER_LEN
  );
  mu_assert(
    "Num of bytes read and written don't match up",
    read == written
  );

  mu_assert(
    "Priv key isn't consistent after serialization",
    mbedtls_mpi_cmp_mpi(priv_key, desered_priv_key) == 0
  );

  // Ensure that we are actually filling the serialization buffer
  for (int i = 0; i < 5; i++) {
    memset(sered_priv_key_2, i, PRIV_KEY_SER_LEN);
    ser_priv_key(sered_priv_key_2, priv_key);
    mu_assert(
      "Entire serialization buffer is not being overwritten",
      memcmp(sered_priv_key, sered_priv_key_2, PRIV_KEY_SER_LEN) == 0
    );
  }

  mbedtls_ecp_keypair_free(keypair);
  free(sered_priv_key);
  free(desered_priv_key);

  return NULL;
}

static char  *test_ser_keypair() {
  mbedtls_ecdsa_context *keypair, *desered_keypair;
  unsigned char *sered_keypair;
  unsigned char sered_keypair_2[KEYPAIR_SER_LEN];
  ssize_t read, written;

  keypair = gen_keys();
  sered_keypair = ser_keypair_alloc(&read, keypair);
  desered_keypair = deser_keypair_alloc(&written, sered_keypair);

  mu_assert(
    "Num of bytes read incorrect",
    read == KEYPAIR_SER_LEN
  );
  mu_assert(
    "Num of bytes read and written don't match up",
    read == written
  );

  mu_assert(
    "Keypair pub key isn't consistent after serialization",
    mbedtls_ecp_point_cmp(&keypair->private_Q, &keypair->private_Q) == 0
  );
  mu_assert(
    "Keypair priv key isn't consistent after serialization",
    mbedtls_mpi_cmp_mpi(&keypair->private_d, &desered_keypair->private_d) == 0
  );

  // Ensure that we are actually filling the serialization buffer
  for (int i = 0; i < 5; i++) {
    memset(sered_keypair_2, i, KEYPAIR_SER_LEN);
    ser_keypair(sered_keypair_2, keypair);
    mu_assert(
      "Entire serialization buffer is not being overwritten",
      memcmp(sered_keypair, sered_keypair_2, KEYPAIR_SER_LEN) == 0
    );
  }

  mbedtls_ecp_keypair_free(keypair);
  free(sered_keypair);
  free(desered_keypair);

  return NULL;
}

static char *all_tests() {
  mu_run_test(test_ser_pub_key);
  mu_run_test(test_ser_priv_key);
  mu_run_test(test_ser_keypair);
  return NULL;
}

int main() {
  char *result = all_tests();
  if (result != NULL) {
    printf("%s\n", result);
  } else {
    printf("ser_key.c passing!\n");
  }
  printf("Tests run: %d\n", tests_run);

  return result != 0;
}
