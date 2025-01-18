#ifndef WIFI_H
#define WIFI_H
#endif

#include <stdio.h>
#include "linenoise/linenoise.h"
#include "esp_vfs_dev.h"
#include "driver/uart.h"
#include "esp_log.h"


void wifi_scan(void);
void initialise_wifi(void);
void deauth(void);