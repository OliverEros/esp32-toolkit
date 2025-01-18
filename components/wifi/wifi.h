#ifndef WIFI_H
#define WIFI_H
#endif

#include <stdio.h>
#include "linenoise/linenoise.h"
#include "esp_vfs_dev.h"
#include "driver/uart.h"
#include "esp_log.h"

// struct FormattedPayload {
//     uint8_t FrameControl;
//     uint8_t Duration;
//     uint8_t RA;
//     uint8_t TA;
//     uint8_t SeqCtl;
// };

void wifi_scan(void);
void initialise_wifi(void);
void deauth(void);
uint8_t get_target_mac_address(uint8_t targetId);
void print_scanned_AP(void);
void PromiscOn(void);