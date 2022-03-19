/**
* @file validate_block.h
* @author Nathan Faber (nfaber@olin.edu.com)
* @brief HEader for functions to handle block validation when a block is received
* @version 0.1
* @date 2022-03-18
* 
*/

#pragma once
#include "base_tx.h"
#include "base_block.h"

/* how should we maintain the set of blocks to validate?
 Seems like a hashmap would be good so we can check the header hash, 
 but also keep order if we are requesting blocks*/

