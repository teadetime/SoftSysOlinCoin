#pragma once

#include "base_tx.h"

#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"

void init_entropy();

char *seed = "42";
mbedtls_entropy_context *entropy = NULL;
mbedtls_ctr_drbg_context *ctr_drbg = NULL;
