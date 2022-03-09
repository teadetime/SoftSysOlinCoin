#pragma once

#include "base_tx.h"

#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/ecdsa.h"

#define CURVE MBEDTLS_ECP_DP_SECP256K1

void init_entropy();
void free_entropy();

mbedtls_ecdsa_context *gen_keys();
size_t write_sig(
    unsigned char *dest, size_t dest_len,
    unsigned char *hash, size_t hash_len,
    mbedtls_ecdsa_context *keys
);
void hash_pub_key(unsigned char *dest, mbedtls_ecdsa_context *key_pair);

extern char *seed;
extern mbedtls_entropy_context *entropy;
extern mbedtls_ctr_drbg_context *ctr_drbg;
