#pragma once

#include <stddef.h>
#include "mbedtls/ecdsa.h"


#define TX_HASH_LEN 32
#define BLOCK_HASH_LEN 32
#define PUB_KEY_HASH_LEN 32
#define PUB_KEY_SER_LEN 65
#define SIGNATURE_LEN 72
#define PRINT_TAB "  "
#define ERR_BUF 1024

const unsigned int desired_num_tx = 10;
 /*
 returns 1 if successful hash of input data

 input_data: data to be hashed
 input_sz: number of bytes in input
 output_hash: unsigned * of size TX_HASH_LEN
 */
int hash_sha256(unsigned char *output_hash, unsigned char *input_data, size_t input_sz); 


/*
Prints out a buffer on len size using hex encoding (twice as many as chars)

title: string title for buffer
buf: buffer to be printed
len: size of buffer in bytes

Example Code:
unsigned char *dest = malloc(TX_HASH_LEN);
unsigned char tx_buf[] = "THIS IS A TEST";
hash_sha256(dest, tx_buf, sizeof(tx_buf));
dump_buf("Hash: ",dest, TX_HASH_LEN);
*/
void dump_buf(char *prefix, const char *title, unsigned char *buf, size_t len);
