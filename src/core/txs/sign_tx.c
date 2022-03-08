#include "sign_tx.h"

#include "mbedtls/ecdsa.h"
#include "mbedtls/sha256.h"

#include <string.h>
#include <stdlib.h>

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
  init_entropy();
  mbedtls_ecdsa_context *keys;
  keys = malloc(sizeof(mbedtls_ecdsa_context));
  mbedtls_ecdsa_init(keys);
  mbedtls_ecdsa_genkey(keys, CURVE, mbedtls_ctr_drbg_random, ctr_drbg);
  return keys;
}

size_t write_sig(
    unsigned char *dest, size_t dest_len,
    unsigned char *hash, size_t hash_len,
    mbedtls_ecdsa_context *keys
) {
    size_t ret_len;
    mbedtls_ecdsa_write_signature(
        keys,
        MBEDTLS_MD_SHA256, hash, hash_len,
        dest, dest_len, &ret_len,
        mbedtls_ctr_drbg_random, ctr_drbg
    );
    return ret_len;
}
