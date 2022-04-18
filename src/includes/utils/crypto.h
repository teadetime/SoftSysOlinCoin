#pragma once

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
 * Signs passed hash using passed key to passed destination
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

/**
 * @brief Validates a signature
 *
 * Validates a signature given a message hash and key pair
 *
 * @param sig Signature buffer to validate
 * @param sig_len Length of the signature buffer
 * @param hash Hash buffer to validate signture with
 * @param hash_len Length of the hash buffer
 * @param key_pair Key pair to validate signture with
 * @return 0 if valid signature, 1 otherwise
 */
int validate_sig(
    unsigned char *sig, size_t sig_len,
    unsigned char *hash, size_t hash_len,
    mbedtls_ecdsa_context *key_pair
);

/**
 * @brief Build a mbed keypair with only a public key, for input unlocking
 * Note this should only be used to copy into statically allocated key pairs 
 * since public keys are prone to being freed often
 * @param ctx key pair previously allocated that still needs initialized and set
 * @param pub_key public key to put into the key pair
 */
void build_ctx_from_public(mbedtls_ecdsa_context *ctx, mbedtls_ecp_point *pub_key);

/* HASHING */

/**
 * @brief Applies sha256 to input data
 *
 * @param output_hash Destination buffer to write to
 * @param input_data Input buffer to hash
 * @param input_sz Size of input buffer
 * @return 0 if succesful
 */
int hash_sha256(unsigned char *output_hash, unsigned char *input_data, size_t input_sz); 

/**
 * @brief Hash public key of a key pair
 *
 * @param dest Destination buffer to serialize to
 * @param key_pair Key pair containing public key to hash
 */
void hash_pub_key(unsigned char *dest, mbedtls_ecdsa_context *key_pair);


/* MISC */

/**
 * @brief Prints a byte buffer in hex
 *
 * Example Code:
 * unsigned char *dest = malloc(TX_HASH_LEN);
 * unsigned char tx_buf[] = "THIS IS A TEST";
 * hash_sha256(dest, tx_buf, sizeof(tx_buf));
 * dump_buf("Hash: ",dest, TX_HASH_LEN);
 *
 * @param prefix Prefix to add before printing
 * @param title Title to be printed
 * @param buf Buffer to be printed in hex
 * @param len Length of buffer to be printed
 */
void dump_buf(char *prefix, const char *title, const unsigned char *buf, size_t len);

/* GLOBALS */

extern char *seed;
extern mbedtls_entropy_context *entropy;
extern mbedtls_ctr_drbg_context *ctr_drbg;
