// # include mbedtls.h
#include <string.h>
#include <constants.h>
#include <stdio.h>
#include "mbedtls/sha256.h"


int hash_sha256(unsigned char * output_hash, unsigned char * input_data, size_t input_sz){
  int ret = 0;
   if( ( ret = mbedtls_sha256( input_data, input_sz, output_hash, 0 ) ) != 0 )
    {
        printf( " failed\n  ! mbedtls_sha256 returned %d\n", ret );
    }
  return ret;
}

void dump_buf( const char *title, unsigned char *buf, size_t len )
{
    printf( "%s", title);
    for(size_t i = 0; i < len; i++ )
        printf("%c%c", "0123456789ABCDEF" [buf[i] / 16],
                       "0123456789ABCDEF" [buf[i] % 16] );
    printf( "\n" );
}