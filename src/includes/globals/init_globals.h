/**
 * @file init_globals.h
 * @author nfaber@olin.edu and efisher@olin.edu
 * @brief Initialize the global datastructures required for a node to run
 * @version 0.1
 * @date 2022-05-06
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

/**
 * @brief Initializes node data structures
 *
 * @param db_env TEST_DB_LOC or PROD_DB_LOC to initialize node
 */
void node_init(char *db_env);

/**
 * @brief Initializes miner data structures
 *
 * Initializes mempool
 */
void miner_init();
