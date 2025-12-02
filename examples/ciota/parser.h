/**
 * @file parser.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-05-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef __PARSER__H__
#define __PARSER__H__

#include "ciot_err.h"

/**
 * @brief Parses command-line arguments and fills the OTA configuration structure.
 * 
 * This function reads the arguments passed to the application, extracts relevant
 * configuration parameters, and populates the `ciota_cfg_t` structure accordingly.
 * 
 * @param argc Argument count from main().
 * @param argv Argument vector from main().
 * @param cfg Pointer to the ciota_cfg_t structure to populate.
 * @return ciot_err_t Error code indicating success or failure of the parsing operation.
 */
ciot_err_t parse_args(int argc, char *argv[], ciota_cfg_t *cfg);

#endif  //!__PARSER__H__