/**
 * @file main.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-05-16
 *
 * @copyright Copyright (c) 2025
 *
 */

#include <stdio.h>
#include "ciot_cmd.h"
#include "ciot_iface.h"

ciot_err_t main(int argc, char **argv)
{
    if (argc == 1)
    {
        // static char *argv_test[] = {
        //     "ciot",                     // argv[0]
        //     "ota",                      // argv[1]
        //     "--sender=http_client",     // argv[2]
        //     "--iface-id=1",             // argv[3]
        //     "--url=http://example.com", // argv[4]
        //     "--force=true",             // argv[5]
        //     "--encrypted=false",        // argv[6]
        //     "--restart=true",           // argv[7]
        //     "--type=1"                  // argv[8]
        // };

        // static char *argv_test[] = {
        //     "ciot",              // argv[0]
        //     "create",            // argv[1]
        //     "--iface-id=0",      // argv[2]
        //     "--iface-type=uart", // argv[3]
        // };

        static char *argv_test[] = {
            "ciot",            // argv[0]
            "ciot",            // argv[1]
            "--get-data=info", // argv[2]
            "--iface-id=0",    // argv[3]
            "--sender=0",      // argv[3]
        };

        argc = sizeof(argv_test) / sizeof(argv_test[0]);
        argv = argv_test;
    }

    ciot_cmd_context_t ctx = {0};
    ciot_err_t err = parse_ciot_cmd(argc, argv, &ctx);
    if (err != CIOT_ERR_OK)
    {
        fprintf(stderr, "Error parsing command line arguments: %s\n", ciot_err_to_message(err));
        return err;
    }
    return CIOT_ERR_OK;
}
