#pragma once
#include <stddef.h>

#define TX_HASH_LEN 32
#define ALL_TX_HASH_LEN 32
#define BLOCK_HASH_LEN 32
#define PUB_KEY_HASH_LEN 32
#define SIGNATURE_LEN 48
#define PRINT_TAB "  "
#define BLOCK_REWARD 100
#define DESIRED_NUM_TX 10
#define HASH_DIFFICULTY 2

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

EXample Code:
unsigned char *dest = malloc(TX_HASH_LEN);
unsigned char tx_buf[] = "THIS IS A TEST";
hash_sha256(dest, tx_buf, sizeof(tx_buf));
dump_buf("Hash: ",dest, TX_HASH_LEN);
*/
void dump_buf(char *prefix, const char *title, unsigned char *buf, size_t len);