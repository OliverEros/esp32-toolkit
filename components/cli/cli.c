#include <stdio.h>
#include "esp_console.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_vfs_dev.h"
#include "esp_mac.h"

#include "wifi.h";




#define ESP_CONSOLE_CONFIG_DEFAULT()           \
{                                          \
    .max_cmdline_length = 256,             \
    .max_cmdline_args = 32,                \
    .heap_alloc_caps = MALLOC_CAP_DEFAULT, \
    .hint_color = 39,                      \
    .hint_bold = 0                         \
}




//Scans for WiFi networks.
static int scan_wifi_networks(int argc, char **argv) {

    wifi_scan();
   return 0;
}

//Deauthenticas a scanned AP - accepts the scan ID as parameter.
static int deauthenticate_AP(int argc, char **argv) {
   deauth();
   return 0;
}

//Lists the last scanned networks.
static int list_scanned_aps(){
    print_scanned_AP();
    return 0;
}

//Gets a scanned AP's MAC address - accepts the scan ID as parameter.
static int get_target_mac(int argc, char **argv){
    get_target_mac_address(0);
    return 0;
}

//Enables promiscios mode
//FIXME - need to be able to interrupt, otherwise prints on the screen forever.
static int promisc_on(){
    PromiscOn();
    return 0;
}



 static const esp_console_cmd_t commands[] = 
 { 
    
    {
        .command = "ws", //use "-" seperated since might implement bluetooth scan in future.
        .help = "Scans for available WiFi networks.",
        .hint = NULL,
        .func = &scan_wifi_networks,
    },

    {
        .command = "wl",
        .help = "Returns scanned APs.",
        .hint = NULL,
        .func = &list_scanned_aps //TODO - Implement function.

    },

    // {
    //     .command = "wc", //NOTE - How about wc4? For more attack types.
    //     .help = "Captures four-way handshake.",
    //     .hint = NULL,
    //     .func = NULL
    //     //.argtable = //REVIEW - Add!
    // },

    {
        .command = "wdeauth",
        .help = "Send deauthenticates packets to a selected WiFi network. ",
        .hint = NULL,
        .func = &deauthenticate_AP,
    },

    {
        .command = "wmac",
        .help = "Gets MAC address of target.",
        .hint = NULL,
        .func = &get_target_mac,
    },

    {   
        .command = "wpromisc",
        .help = "Initiates promiscious mode.",
        .hint = NULL,
        .func = &promisc_on,
    }

      
};



// Automated command registration
void register_commands(void) {

    for(uint8_t i = 0; i < sizeof(commands) / sizeof(commands[0]); i++){
        ESP_ERROR_CHECK(esp_console_cmd_register(&commands[i]));
    }
}


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

