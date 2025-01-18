#include <stdio.h>
#include "esp_console.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_vfs_dev.h"
#include "esp_mac.h"

#include "wifi.h";


// Tag for logging
static const char *TAG = "CLI";


struct cli
{
    /* data */
};

// Example command callback
static int scan_wifi_networks(int argc, char **argv) {
    wifi_scan();
   return 0;
}

static int deauthenticate_wifi_network(int argc, char **argv) {
   deauth();
   return 0;
}

#define ESP_CONSOLE_CONFIG_DEFAULT()           \
{                                          \
    .max_cmdline_length = 256,             \
    .max_cmdline_args = 32,                \
    .heap_alloc_caps = MALLOC_CAP_DEFAULT, \
    .hint_color = 39,                      \
    .hint_bold = 0                         \
}

 static const esp_console_cmd_t commands[] = 
 { 
    
    {
        .command = "ws", //use "-" seperated since might implement bluetooth scan in future.
        .help = "Scans for available WiFi networks.",
        .hint = NULL,
        .func = &scan_wifi_networks,
    },

    // {
    //     .command = "wl",
    //     .help = "Returns information for previously scanned networks.",
    //     .hint = NULL,
    //     .func = NULL //TODO - Implement function.

    // },

    // {
    //     .command = "wc", //NOTE - How about wc4? For more attack types.
    //     .help = "Captures four-way handshake.",
    //     .hint = NULL,
    //     .func = NULL
    //     //.argtable = //REVIEW - Add!
    // },

    {
        .command = "w-deauth",
        .help = "Send deauthenticates packets to a selected WiFi network. ",
        .hint = NULL,
        .func = &deauthenticate_wifi_network,
    }
      
};



// Command registration
void register_commands(void) {

    // esp_console_cmd_t *ptr;

    // for(ptr = commands; ptr < commands + sizeof(commands); ++ptr) 
    // {
    //     ESP_ERROR_CHECK(esp_console_cmd_register(ptr));
    // };

    ESP_ERROR_CHECK(esp_console_cmd_register(&commands[0]));
    ESP_ERROR_CHECK(esp_console_cmd_register(&commands[1]));
}

// Initialize consoleESP_ERROR_CHECK failed: esp_err_t 0x103 (ESP_ERR_INVALID_STATE) at 0x400d765d

void initialize_console(void) {
    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    /* Prompt to be printed before each line.
     * This can be customized, made dynamic, etc.
     */
    repl_config.prompt = "esp_32 >>";
    repl_config.max_cmdline_length = 50;

    /* Register commands */
    esp_console_register_help_command();
    register_commands();
   
    // install console REPL environment

#if CONFIG_ESP_CONSOLE_UART
    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&uart_config, &repl_config, &repl));
#elif CONFIG_ESP_CONSOLE_USB_CDC
    esp_console_dev_usb_cdc_config_t cdc_config = ESP_CONSOLE_DEV_CDC_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_usb_cdc(&cdc_config, &repl_config, &repl));
#elif CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
    esp_console_dev_usb_serial_jtag_config_t usbjtag_config = ESP_CONSOLE_DEV_USB_SERIAL_JTAG_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_usb_serial_jtag(&usbjtag_config, &repl_config, &repl));
#endif

    ESP_ERROR_CHECK(esp_console_start_repl(repl));
   
}

