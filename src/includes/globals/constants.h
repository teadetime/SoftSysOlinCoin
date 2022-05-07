/**
 * @file constants.h
 * @author nfaber@olin.edu and efisher@olin.edu
 * @brief Constants used within the project, character buffer sizes etc
 * @version 0.1
 * @date 2022-05-06
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

/**
 * @brief Buffer length for SHA operations
 * 
 */
#define SHA_HASH_LEN 32

/**
 * @brief Hashed Transaction length
 * 
 */
#define TX_HASH_LEN SHA_HASH_LEN

/**
 * @brief All Transactions Hash length
 * 
 */
#define ALL_TX_HASH_LEN SHA_HASH_LEN

/**
 * @brief Block Hash length
 * 
 */
#define BLOCK_HASH_LEN SHA_HASH_LEN

/**
 * @brief Public Key Hash length
 * 
 */
#define PUB_KEY_HASH_LEN SHA_HASH_LEN

/**
 * @brief Length of ECDSA Signature
 * 
 */
#define SIGNATURE_LEN 72

/**
 * @brief Error Buffer size
 * 
 */
#define ERR_BUF 1024

/**
 * @brief Block Mining Reward
 * 
 */
#define BLOCK_REWARD 100

/**
 * @brief Number of Transactions to include in a block
 * 
 */
#define DESIRED_NUM_TX 10

/**
 * @brief Number of 0's required for the hash of a block
 * 
 */
#define HASH_DIFFICULTY 2

/**
 * @brief Tab Characters for use in printing
 * 
 */
#define PRINT_TAB "  "

/**
 * @brief Line Break for printing
 * 
 */
#define LINE_BREAK "================================\n"

/**
 * @brief Soft Line Break
 * 
 */
#define SOFT_LINE_BREAK "--------------------------------\n"

/**
 * @brief Local location of database folders
 * 
 */
#define LOCAL_LOCATION "/.Olincoin"
