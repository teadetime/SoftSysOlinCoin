#include "constants.h"
#include "ser_key.h"
#include "crypto.h"

#include "mbedtls/ecp.h"
#include "mbedtls/error.h"

#include <stdlib.h>
#include <string.h>

#define INIT_GROUP(group) \
  { \
    mbedtls_ecp_group_init(&group); \
    mbedtls_ecp_group_load(&group, CURVE); \
  }

#define RETURN_SER(data, ret, ptr) \
  { \
    if (ptr != NULL) \
      *ptr = ret; \
    if (ret == -1) { \
      free(data); \
      return NULL; \
    } \
    return data; \
  }

/******************************************************************************
 * Pub Keys
 ******************************************************************************/

ssize_t ser_pub_key(unsigned char *dest, mbedtls_ecp_point *key) {
  size_t num_bytes;
  char buf[ERR_BUF];
  int err;
  mbedtls_ecp_group group;

  INIT_GROUP(group)
  err = mbedtls_ecp_point_write_binary(
    &group, key,
    MBEDTLS_ECP_PF_UNCOMPRESSED,
    &num_bytes, dest, PUB_KEY_SER_LEN
  );
  if (err != 0) {
    mbedtls_strerror(err, buf, ERR_BUF);
    fprintf(stderr, "pub key serialization error: %s\n", buf);
    return -1;
  }
  return num_bytes;
}

unsigned char *ser_pub_key_alloc(ssize_t *written, mbedtls_ecp_point *key) {
  unsigned char *data;
  ssize_t ret;
  data = malloc(PUB_KEY_SER_LEN);
  ret = ser_pub_key(data, key);
  RETURN_SER(data, ret, written)
}

ssize_t deser_pub_key(mbedtls_ecp_point *dest, unsigned char *src) {
  char buf[ERR_BUF];
  int err;
  mbedtls_ecp_group group;

  INIT_GROUP(group)
  err = mbedtls_ecp_point_read_binary(&group, dest, src, PUB_KEY_SER_LEN);
  if (err != 0) {
    mbedtls_strerror(err, buf, ERR_BUF);
    printf("pub key deserialization error: %s\n", buf);
    return -1;
  }
  return PUB_KEY_SER_LEN;
}

mbedtls_ecp_point *deser_pub_key_alloc(ssize_t *read, unsigned char *src) {
  mbedtls_ecp_point *pub_key;
  ssize_t ret;
  pub_key = malloc(sizeof(mbedtls_ecp_point));
  mbedtls_ecp_point_init(pub_key);
  ret = deser_pub_key(pub_key, src);
  RETURN_SER(pub_key, ret, read)
}

/******************************************************************************
 * Private Keys
 ******************************************************************************/

ssize_t ser_priv_key(unsigned char *dest, mbedtls_mpi *key) {
  char buf[ERR_BUF];
  int err;
  mbedtls_ecp_group group;

  INIT_GROUP(group)
  err = mbedtls_mpi_write_binary(key, dest, PRIV_KEY_SER_LEN);
  if (err != 0) {
    mbedtls_strerror(err, buf, ERR_BUF);
    fprintf(stderr, "priv key serialization error: %s\n", buf);
    return -1;
  }
  return PRIV_KEY_SER_LEN;
}

unsigned char *ser_priv_key_alloc(ssize_t *written, mbedtls_mpi *key) {
  unsigned char *data;
  ssize_t ret;
  data = malloc(PRIV_KEY_SER_LEN);
  ret = ser_priv_key(data, key);
  RETURN_SER(data, ret, written)
}

ssize_t deser_priv_key(mbedtls_mpi *dest, unsigned char *src) {
  char buf[ERR_BUF];
  int err;
  mbedtls_ecp_group group;

  INIT_GROUP(group)
  err = mbedtls_mpi_read_binary(dest, src, PRIV_KEY_SER_LEN);
  if (err != 0) {
    mbedtls_strerror(err, buf, ERR_BUF);
    printf("priv key deserialization error: %s\n", buf);
    return -1;
  }
  return PRIV_KEY_SER_LEN;
}

mbedtls_mpi *deser_priv_key_alloc(ssize_t *read, unsigned char *src) {
  mbedtls_mpi *priv_key;
  ssize_t ret;
  priv_key = malloc(sizeof(mbedtls_mpi));
  mbedtls_mpi_init(priv_key);
  ret = deser_priv_key(priv_key, src);
  RETURN_SER(priv_key, ret, read)
}

/******************************************************************************
 * Key Pairs
 ******************************************************************************/

ssize_t ser_keypair(unsigned char *dest, mbedtls_ecdsa_context *keypair) {
  unsigned char *priv_key = dest + ser_pub_key(dest, &keypair->private_Q);
  unsigned char *end = priv_key + ser_priv_key(priv_key, &keypair->private_d);
  return end - dest;
}

unsigned char *ser_keypair_alloc(ssize_t *written, mbedtls_ecdsa_context *keypair) {
  unsigned char *data;
  ssize_t ret;
  data = malloc(KEYPAIR_SER_LEN);
  ret = ser_keypair(data, keypair);
  RETURN_SER(data, ret, written)
}

ssize_t deser_keypair(mbedtls_ecdsa_context *dest, unsigned char *src) {
  unsigned char *priv_key = src + deser_pub_key(&dest->private_Q, src);
  unsigned char *end = priv_key + deser_priv_key(&dest->private_d, priv_key);
  return end - src;
}

mbedtls_ecdsa_context *deser_keypair_alloc(ssize_t *read, unsigned char *src) {
  mbedtls_ecdsa_context *keypair;
  ssize_t ret;
  keypair = malloc(sizeof(mbedtls_ecdsa_context));
  mbedtls_ecdsa_init(keypair);
  ret = deser_keypair(keypair, src);
  RETURN_SER(keypair, ret, read)
}
