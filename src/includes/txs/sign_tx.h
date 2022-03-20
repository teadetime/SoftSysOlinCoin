#pragma once

#include "base_tx.h"

#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/ecdsa.h"

#define CURVE MBEDTLS_ECP_DP_SECP256K1

/* KEY GENERATION */

/**
 * @brief Initializes the entropy objects
 *
 * Builds the entropy objects used in key generation and signing.
 */
void init_entropy();

/**
 * @brief Frees the entropy objects
 *
 * Frees the entropy objects used in key generation and signing.
 */
void free_entropy();

/**
 * @brief Generates a ECC key pair
 *
 * Generates a ECC key pair of curve specified by the CURVE constant.
 * Initializes entropy objects if they aren't initialized.
 *
 * @return A new key pair
 */
mbedtls_ecdsa_context *gen_keys();

/* SIGNATURE HANDLING */

/**
 * @brief Writes a signature
 *
 * Signs passed hash using passed key to passed destination.
 *
 * @param dest Destination buffer to write to
 * @param dest_len Length of the destination buffer
 * @param hash Hash buffer to sign
 * @param hash_len Length of the hash buffer
 * @param keys Key pair to sign with
 * @return Number of bytes written
 */
size_t write_sig(
    unsigned char *dest, size_t dest_len,
    unsigned char *hash, size_t hash_len,
    mbedtls_ecdsa_context *keys
);

/* PUB KEY HANDLING */

/**
 * @brief Serialize a public key
 *
 * @param dest Destination buffer to serialize to
 * @param point Public key to serialize. Can be accessed from key pair as
 *   key_pair->MBEDTLS_PRIVATE(Q)
 * @param grp Public key group. Can be accessed from key pair as
 *   key_pair->MBEDTLS_PRIVATE(grp)
 * @return Number of bytes written
 */
size_t ser_pub_key(unsigned char *dest, mbedtls_ecp_point *point, mbedtls_ecp_group *grp);

/**
 * @brief Deserialize a public key
 *
 * Deserializes a public key serialized with ser_pub_key()
 *
 * @param dest Destination key to write to
 * @param grp Group of key we are reading
 * @param data Source buffer containing serialized key
 */
void deser_pub_key(mbedtls_ecp_point *dest, mbedtls_ecp_group *grp, unsigned char *data);

/**
 * @brief Hash public key of a key pair
 *
 * @param dest Destination buffer to serialize to
 * @param key_pair Key pair containing public key to hash
 */
void hash_pub_key(unsigned char *dest, mbedtls_ecdsa_context *key_pair);

extern char *seed;
extern mbedtls_entropy_context *entropy;
extern mbedtls_ctr_drbg_context *ctr_drbg;
