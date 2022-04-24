#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <setjmp.h>
#include <cmocka.h>

#include "ser_key.h"
#include "fixtures_wallet.h"

static void test_ser_pub_key(void **state) {
  mbedtls_ecdsa_context *keypair;
  mbedtls_ecp_point *pub_key, *desered_pub_key;
  unsigned char *sered_pub_key;
  ssize_t read, written;

  keypair = *state;
  pub_key = &keypair->private_Q;
  sered_pub_key = ser_pub_key_alloc(&read, pub_key);
  desered_pub_key = deser_pub_key_alloc(&written, sered_pub_key);

  assert_int_equal(read, PUB_KEY_SER_LEN);
  assert_int_equal(read, written);
  assert_true(mbedtls_ecp_point_cmp(pub_key, desered_pub_key) == 0);

  free(sered_pub_key);
  free(desered_pub_key);
}

static void test_ser_pub_key_fill(void **state) {
  mbedtls_ecdsa_context *keypair;
  mbedtls_ecp_point *pub_key;
  unsigned char *sered_pub_key;
  unsigned char sered_pub_key_2[PUB_KEY_SER_LEN];

  keypair = *state;
  pub_key = &keypair->private_Q;
  sered_pub_key = ser_pub_key_alloc(NULL, pub_key);

  // Ensure that we are actually filling the serialization buffer
  for (int i = 0; i < 5; i++) {
    memset(sered_pub_key_2, i, PUB_KEY_SER_LEN);
    ser_pub_key(sered_pub_key_2, pub_key);
    assert_memory_equal(sered_pub_key, sered_pub_key_2, PUB_KEY_SER_LEN);
  }

  free(sered_pub_key);
}

static void test_ser_priv_key(void **state) {
  mbedtls_ecdsa_context *keypair;
  mbedtls_mpi *priv_key, *desered_priv_key;
  unsigned char *sered_priv_key;
  ssize_t read, written;

  keypair = *state;
  priv_key = &keypair->private_d;
  sered_priv_key = ser_priv_key_alloc(&read, priv_key);
  desered_priv_key = deser_priv_key_alloc(&written, sered_priv_key);

  assert_int_equal(read, PRIV_KEY_SER_LEN);
  assert_int_equal(read, written);
  assert_true(mbedtls_mpi_cmp_mpi(priv_key, desered_priv_key) == 0);

  free(sered_priv_key);
  free(desered_priv_key);
}

static void test_ser_priv_key_fill(void **state) {
  mbedtls_ecdsa_context *keypair;
  mbedtls_mpi *priv_key;
  unsigned char *sered_priv_key;
  unsigned char sered_priv_key_2[PRIV_KEY_SER_LEN];

  keypair = *state;
  priv_key = &keypair->private_d;
  sered_priv_key = ser_priv_key_alloc(NULL, priv_key);

  // Ensure that we are actually filling the serialization buffer
  for (int i = 0; i < 5; i++) {
    memset(sered_priv_key_2, i, PRIV_KEY_SER_LEN);
    ser_priv_key(sered_priv_key_2, priv_key);
    assert_memory_equal(sered_priv_key, sered_priv_key_2, PRIV_KEY_SER_LEN);
  }

  free(sered_priv_key);
}

static void test_ser_keypair(void **state) {
  mbedtls_ecdsa_context *keypair, *desered_keypair;
  unsigned char *sered_keypair;
  ssize_t read, written;

  keypair = *state;
  sered_keypair = ser_keypair_alloc(&read, keypair);
  desered_keypair = deser_keypair_alloc(&written, sered_keypair);

  assert_int_equal(read, KEYPAIR_SER_LEN);
  assert_int_equal(read, written);

  assert_true(mbedtls_ecp_point_cmp(
      &keypair->private_Q,
      &keypair->private_Q
    ) == 0
  );
  assert_true(mbedtls_mpi_cmp_mpi(
      &keypair->private_d,
      &desered_keypair->private_d
    ) == 0
  );
  assert_true(mbedtls_ecp_check_privkey(
      &keypair->private_grp,
      &keypair->private_d
    ) == 0
  );  // Proxy for checking group, breaks if group if wrong

  free(sered_keypair);
  free(desered_keypair);
}

static void test_ser_keypair_fill(void **state) {
  mbedtls_ecdsa_context *keypair;
  unsigned char *sered_keypair;
  unsigned char sered_keypair_2[KEYPAIR_SER_LEN];

  keypair = *state;
  sered_keypair = ser_keypair_alloc(NULL, keypair);

  // Ensure that we are actually filling the serialization buffer
  for (int i = 0; i < 5; i++) {
    memset(sered_keypair_2, i, KEYPAIR_SER_LEN);
    ser_keypair(sered_keypair_2, keypair);
    assert_memory_equal(sered_keypair, sered_keypair_2, KEYPAIR_SER_LEN);
  }

  free(sered_keypair);
}

int main() {
  const struct CMUnitTest tests[] = {
    cmocka_unit_test_setup_teardown(
      test_ser_pub_key,
      fixture_setup_unlinked_keypair,
      fixture_teardown_unlinked_keypair
    ),
    cmocka_unit_test_setup_teardown(
      test_ser_pub_key_fill,
      fixture_setup_unlinked_keypair,
      fixture_teardown_unlinked_keypair
    ),
    cmocka_unit_test_setup_teardown(
      test_ser_priv_key,
      fixture_setup_unlinked_keypair,
      fixture_teardown_unlinked_keypair
    ),
    cmocka_unit_test_setup_teardown(
      test_ser_priv_key_fill,
      fixture_setup_unlinked_keypair,
      fixture_teardown_unlinked_keypair
    ),
    cmocka_unit_test_setup_teardown(
      test_ser_keypair,
      fixture_setup_unlinked_keypair,
      fixture_teardown_unlinked_keypair
    ),
    cmocka_unit_test_setup_teardown(
      test_ser_keypair_fill,
      fixture_setup_unlinked_keypair,
      fixture_teardown_unlinked_keypair
    ),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
