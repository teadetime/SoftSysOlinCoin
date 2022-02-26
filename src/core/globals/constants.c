// # include mbedtls.h
#include <string.h>
#include <constants.h>

// TODO: Actually implelement.
int hash_sha256(unsigned char * output_hash, unsigned char * input_data, unsigned int input_sz){
  memcpy(output_hash, input_data, TX_HASH_LEN);
  return 1;
}
