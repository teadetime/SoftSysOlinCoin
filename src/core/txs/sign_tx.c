#include "sign_tx.h"

#include "mbedtls/ecdsa.h"
#include "mbedtls/sha256.h"
#include "mbedtls/error.h"

#include <string.h>
#include <stdlib.h>

#define ERR_BUF 1024

char *seed = "42";
mbedtls_entropy_context *entropy = NULL;
mbedtls_ctr_drbg_context *ctr_drbg = NULL;

void init_entropy() {
  // Immediatly return if already built
  if (entropy != NULL || ctr_drbg != NULL)
    return;

  entropy = malloc(sizeof(mbedtls_entropy_context));
  ctr_drbg = malloc(sizeof(mbedtls_ctr_drbg_context));

  mbedtls_entropy_init(entropy);
  mbedtls_ctr_drbg_init(ctr_drbg);

  mbedtls_ctr_drbg_seed(
    ctr_drbg, mbedtls_entropy_func, entropy,
    (const unsigned char *) seed, strlen(seed)
  );
}

void free_entropy() {
  if (entropy != NULL)
    free(entropy);
  if (ctr_drbg != NULL)
    free(ctr_drbg);
  entropy = NULL;
  ctr_drbg = NULL;
}

mbedtls_ecdsa_context *gen_keys() {
  char buf[ERR_BUF];
  int err;

  init_entropy();
  mbedtls_ecdsa_context *keys;
  keys = malloc(sizeof(mbedtls_ecdsa_context));
  mbedtls_ecdsa_init(keys);
  err = mbedtls_ecdsa_genkey(keys, CURVE, mbedtls_ctr_drbg_random, ctr_drbg);
  if (err != 0) {
    mbedtls_strerror(err, buf, ERR_BUF);
    printf("Error! %s\n", buf);
    exit(1);
  }

  return keys;
}

size_t write_sig(
    unsigned char *dest, size_t dest_len,
    unsigned char *hash, size_t hash_len,
    mbedtls_ecdsa_context *keys
) {
    size_t ret_len;
    int err;
    char buf[ERR_BUF];

    err = mbedtls_ecdsa_write_signature(
        keys,
        MBEDTLS_MD_SHA256, hash, hash_len,
        dest, dest_len, &ret_len,
        mbedtls_ctr_drbg_random, ctr_drbg
    );
    if (err != 0) {
      mbedtls_strerror(err, buf, ERR_BUF);
      printf("Error! %s\n", buf);
      exit(1);
    }

    return ret_len;
}


size_t ser_pub_key(
    unsigned char *dest, mbedtls_ecp_point *point, mbedtls_ecp_group *grp
) {
  size_t num_bytes;
  char buf[ERR_BUF];
  int err;

  err = mbedtls_ecp_point_write_binary(
    grp, point,
    MBEDTLS_ECP_PF_UNCOMPRESSED,
    &num_bytes, dest, PUB_KEY_SER_LEN
  );
  if (err != 0) {
    mbedtls_strerror(err, buf, ERR_BUF);
    printf("Serialize pub key error! %s\n", buf);
    exit(1);
  }

  return num_bytes;
}

void deser_pub_key(mbedtls_ecp_point *dest, mbedtls_ecp_group *grp, unsigned char *data) {
  char buf[ERR_BUF];
  int err;

  err = mbedtls_ecp_point_read_binary(grp, dest, data, PUB_KEY_SER_LEN);
  if (err != 0) {
    mbedtls_strerror(err, buf, ERR_BUF);
    printf("Deserialize pub key error! %s\n", buf);
    exit(1);
  }
}

void hash_pub_key(unsigned char *dest, mbedtls_ecdsa_context *key_pair) {
  unsigned char ser_key[PUB_KEY_SER_LEN];
  size_t num_bytes;

  num_bytes = ser_pub_key(
    ser_key,
    &(key_pair->MBEDTLS_PRIVATE(Q)),
    &(key_pair->MBEDTLS_PRIVATE(grp))
  );
  hash_sha256(dest, ser_key, num_bytes);
}
