#pragma once

#include "mbedtls/ecdsa.h"

#define PUB_KEY_SER_LEN 65
#define PRIV_KEY_SER_LEN 32
#define KEYPAIR_SER_LEN PUB_KEY_SER_LEN + PRIV_KEY_SER_LEN

/******************************************************************************
 * Pub Keys
 ******************************************************************************/

/**
 * @brief Serialize a public key
 *
 * @param dest Buffer of length PUB_KEY_SER_LEN to write to
 * @param key mbedtls_ecp_point to serialize
 * @return Number of bytes written if succesfull, -1 otherwise
 */
ssize_t ser_pub_key(unsigned char *dest, mbedtls_ecp_point *key);

/**
 * @brief Allocates memory and serializes a public key
 *
 * @param written Stores number of bytes written if succesfull, -1 otherwise
 * @param key mbedtls_ecp_point to serialize
 * @return Serialized public key of length PUB_KEY_SER_LEN if succesfull, NULL
 *   otherwise
 */
unsigned char *ser_pub_key_alloc(ssize_t *written, mbedtls_ecp_point *key);

/**
 * @brief Deserialize a public key
 *
 * @param dest initialized mbedtls_ecp_point to write to
 * @param src Buffer of length PUB_KEY_SER_LEN to read from
 * @return Number of bytes read if succesfull, -1 otherwise
 */
ssize_t deser_pub_key(mbedtls_ecp_point *dest, unsigned char *src);

/**
 * @brief Allocate memory and deserialize a public key
 *
 * @param read Stores number of bytes read if succesfull, -1 otherwise
 * @param src Buffer of length PUB_KEY_SER_LEN to read from
 * @return Deserialized public key if succesfull, NULL otherwise
 */
mbedtls_ecp_point *deser_pub_key_alloc(ssize_t *read, unsigned char *src);

/******************************************************************************
 * Private Keys
 ******************************************************************************/

/**
 * @brief Serialize a private key
 *
 * @param dest Buffer of length PRIV_KEY_SER_LEN to write to
 * @param key mbedtls_mpi to serialize
 * @return Number of bytes written if succesfull, -1 otherwise
 */
ssize_t ser_priv_key(unsigned char *dest, mbedtls_mpi *key);

/**
 * @brief Allocates memory and serializes a private key
 *
 * @param written Stores number of bytes written if succesfull, -1 otherwise
 * @param key mbedtls_mpi to serialize
 * @return Serialized private key of length PRIV_KEY_SER_LEN if succesfull, NULL
 *   otherwise
 */
unsigned char *ser_priv_key_alloc(ssize_t *written, mbedtls_mpi *key);

/**
 * @brief Deserialize a private key
 *
 * @param dest initialized mbedtls_mpi to write to
 * @param src Buffer of length PRIV_KEY_SER_LEN to read from
 * @return Number of bytes read if succesfull, -1 otherwise
 */
ssize_t deser_priv_key(mbedtls_mpi *dest, unsigned char *src);

/**
 * @brief Allocate memory and deserialize a private key
 *
 * @param read Stores number of bytes read if succesfull, -1 otherwise
 * @param src Buffer of length PRIV_KEY_SER_LEN to read from
 * @return Deserialized private key if succesfull, NULL otherwise
 */
mbedtls_mpi *deser_priv_key_alloc(ssize_t *read, unsigned char *src);

/******************************************************************************
 * Key Pairs
 ******************************************************************************/

/**
 * @brief Serialize a keypair
 *
 * @param dest Buffer of length KEY_PAIR_SER_LEN to write to
 * @param keypair mbedtls_ecdsa_context to serialize
 * @return Number of bytes written if succesfull, -1 otherwise
 */
ssize_t ser_keypair(unsigned char *dest, mbedtls_ecdsa_context *keypair);

/**
 * @brief Allocates memory and serializes an mbedtls_ecdsa_context
 *
 * @param written Stores number of bytes written if succesfull, -1 otherwise
 * @param keypair mbedtls_ecdsa_context to serialize
 * @return Serialized keypair of length KEY_PAIR_SER_LEN if succesfull, NULL
 *   otherwise
 */
unsigned char *ser_keypair_alloc(ssize_t *written, mbedtls_ecdsa_context *keypair);

/**
 * @brief Deserialize an mbedtls_ecdsa_context
 *
 * @param dest initiialized mbedtls_ecdsa_context to write to
 * @param src Buffer of length KEY_PAIR_SER_LEN to read from
 * @return Number of bytes read if succesfull, -1 otherwise
 */
ssize_t deser_keypair(mbedtls_ecdsa_context *dest, unsigned char *src);

/**
 * @brief Allocate memory and deserialize an mbedtls_ecdsa_context
 *
 * @param read Stores number of bytes read if succesfull, -1 otherwise
 * @param src Buffer of length KEY_PAIR_SER_LEN to read from
 * @return Deserialized keypair if succesfull, NULL otherwise
 */
mbedtls_ecdsa_context *deser_keypair_alloc(ssize_t *read, unsigned char *src);
