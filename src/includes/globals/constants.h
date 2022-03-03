#pragma once

#include <stddef.h>

#define TX_HASH_LEN 32
#define BLOCK_HASH_LEN 32


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
dump_buf( "Hash: ", output_hash, TX_HASH_LEN );
*/
void dump_buf( const char *title, unsigned char *buf, size_t len );