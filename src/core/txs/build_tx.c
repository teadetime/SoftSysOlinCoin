#include "build_tx.h"

#include "mbedtls/ecdsa.h"
#include "mbedtls/sha256.h"

#include <string.h>
#include <stdlib.h>

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
