#include "crypto.h"
#include "base_tx.h"
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
    printf("Gen key error! %s\n", buf);
    exit(1);
  }

  return keys;
}

size_t write_sig(
    unsigned char *dest, size_t dest_len,
    unsigned char *hash, size_t hash_len,
    mbedtls_ecdsa_context *key_pair
) {
    size_t ret_len;
    int err;
    char buf[ERR_BUF];

    err = mbedtls_ecdsa_write_signature(
        key_pair,
        MBEDTLS_MD_SHA256, hash, hash_len,
        dest, dest_len, &ret_len,
        mbedtls_ctr_drbg_random, ctr_drbg
    );
    if (err != 0) {
      mbedtls_strerror(err, buf, ERR_BUF);
      printf("Write signature error! %s\n", buf);
      exit(1);
    }

    return ret_len;
}

int validate_sig(
    unsigned char *sig, size_t sig_len,
    unsigned char *hash, size_t hash_len,
    mbedtls_ecdsa_context *key_pair
) {
  int err;
  char buf[ERR_BUF];

  err = mbedtls_ecdsa_read_signature(
    key_pair,
    hash, hash_len,
    sig, sig_len
  );

  if (err == 0)
    return 0;
  else if (err == MBEDTLS_ERR_ECP_BAD_INPUT_DATA)
    return 1;
  else {
    mbedtls_strerror(err, buf, ERR_BUF);
    // printf("Validate signature error! %s\n", buf);
    // exit(1);
    return 1;
  }
}

void build_ctx_from_public(mbedtls_ecdsa_context *ctx, mbedtls_ecp_point *pub_key){
  mbedtls_ecdsa_init(ctx);
  mbedtls_ecp_group_load(&ctx->private_grp, CURVE);
  ctx->private_Q = *pub_key;
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

int hash_sha256(unsigned char * output_hash, unsigned char * input_data, size_t input_sz) {
  int ret = 0;
  if ((ret = mbedtls_sha256(input_data, input_sz, output_hash, 0)) != 0) {
    printf( " failed\n  ! mbedtls_sha256 returned %d\n", ret );
  }
  return ret;
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

void dump_buf(char *prefix, const char *title, unsigned char *buf, size_t len) {
    printf( "%s%s",prefix, title);
    for(size_t i = 0; i < len; i++ )
        printf("%c%c", "0123456789ABCDEF" [buf[i] / 16],
                       "0123456789ABCDEF" [buf[i] % 16] );
    printf( "\n" );
}