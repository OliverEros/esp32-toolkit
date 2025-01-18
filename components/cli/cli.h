#ifndef CLI_H
#define CLI_H

#include <stdio.h>
#include "esp_console.h"
#include "linenoise/linenoise.h"
#include "esp_vfs_dev.h"
#include "driver/uart.h"
#include "esp_log.h"

#ifdef __cplusplus
extern "C" {
#endif

// Define a tag for logging
#define CLI_TAG "CLI"

// CLI structure
struct cli {
    // Add fields if necessary
};

/**
 * @brief Callback function for scanning WiFi networks.
 * 
 * @param argc Number of arguments.
 * @param argv Array of arguments.
 * @return int Return 0 on success, otherwise an error code.
 */
int scan_wifi_networks(int argc, char **argv);

/**
 * @brief Callback function for deauthenticating a WiFi network.
 * 
 * @param argc Number of arguments.
 * @param argv Array of arguments.
 * @return int Return 0 on success, otherwise an error code.
 */
int deauthenticate_AP(int argc, char **argv);

/**
 * @brief Register custom commands for the CLI.
 */
void register_commands(void);

/**
 * @brief Initialize the console for CLI operation.
 */
void initialize_console(void);

/**
 * @brief Main task function for the CLI.
 */
void cli_task(void);

#ifdef __cplusplus
}
#endif

#endif // CLI_H
