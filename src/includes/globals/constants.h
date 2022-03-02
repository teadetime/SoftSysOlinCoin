#pragma once

#define TX_HASH_LEN 32
#define BLOCK_HASH_LEN 32

 /*
 returns 1 if successful hash of input data

 input_data: data to be hashed
 input_sz: number of bytes in input
 output_hash: unsigned * of size TX_HASH_LEN
 */
int hash_sha256(unsigned char * output_hash, unsigned char * input_data, unsigned int input_sz); 
