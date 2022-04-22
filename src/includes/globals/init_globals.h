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
