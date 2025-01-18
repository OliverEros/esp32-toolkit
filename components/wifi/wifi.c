/* Scan Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

/*
    This example shows how to scan for available set of APs.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "regex.h"
#include "string.h"

//Custom headers
#include "cli.h"
#include "webserver.h"

#define DEFAULT_SCAN_LIST_SIZE CONFIG_EXAMPLE_SCAN_LIST_SIZE

 const char *TAG = "ESP32";

 //Structure of payload.
// static struct FormattedPayload {
//     uint8_t FrameControl[2];
//     uint8_t Duration[2];
//     uint8_t RA[6];
//     uint8_t TA[6];
//     uint8_t SeqCtl[2];
// } PayLoad;

static struct Scanner
{
   wifi_ap_record_t Clients[10];
   uint16_t maxScannedAP;
   bool hasScanned;
   bool promiscOn;

} scanner;


static void configure_wifi_settings(void){
    scanner.maxScannedAP    = 10;
    scanner.hasScanned      = false;
    scanner.promiscOn       = false;
}

static void print_wifi_config(){
    
    ESP_LOGI(TAG, "============= WIFI CONFIG ==================");
    ESP_LOGI(TAG, "scanner.maScannedAP: %d", scanner.maxScannedAP);
    ESP_LOGI(TAG, "scanner.Clients [SIZE]: %d \n\n", sizeof(scanner.Clients));
}


static void print_auth_mode(int authmode)
{
    switch (authmode) {
    case WIFI_AUTH_OPEN:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_OPEN");
        break;
    case WIFI_AUTH_OWE:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_OWE");
        break;
    case WIFI_AUTH_WEP:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WEP");
        break;
    case WIFI_AUTH_WPA_PSK:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA_PSK");
        break;
    case WIFI_AUTH_WPA2_PSK:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA2_PSK");
        break;
    case WIFI_AUTH_WPA_WPA2_PSK:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA_WPA2_PSK");
        break;
    case WIFI_AUTH_ENTERPRISE:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_ENTERPRISE");
        break;
    case WIFI_AUTH_WPA3_PSK:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA3_PSK");
        break;
    case WIFI_AUTH_WPA2_WPA3_PSK:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA2_WPA3_PSK");
        break;
    case WIFI_AUTH_WPA3_ENT_192:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA3_ENT_192");
        break;
    default:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_UNKNOWN");
        break;
    }
}

static void print_cipher_type(int pairwise_cipher, int group_cipher)
{
    switch (pairwise_cipher) {
    case WIFI_CIPHER_TYPE_NONE:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_NONE");
        break;
    case WIFI_CIPHER_TYPE_WEP40:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_WEP40");
        break;
    case WIFI_CIPHER_TYPE_WEP104:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_WEP104");
        break;
    case WIFI_CIPHER_TYPE_TKIP:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_TKIP");
        break;
    case WIFI_CIPHER_TYPE_CCMP:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_CCMP");
        break;
    case WIFI_CIPHER_TYPE_TKIP_CCMP:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_TKIP_CCMP");
        break;
    case WIFI_CIPHER_TYPE_AES_CMAC128:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_AES_CMAC128");
        break;
    case WIFI_CIPHER_TYPE_SMS4:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_SMS4");
        break;
    case WIFI_CIPHER_TYPE_GCMP:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_GCMP");
        break;
    case WIFI_CIPHER_TYPE_GCMP256:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_GCMP256");
        break;
    default:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_UNKNOWN");
        break;
    }

    switch (group_cipher) {
    case WIFI_CIPHER_TYPE_NONE:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_NONE");
        break;
    case WIFI_CIPHER_TYPE_WEP40:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_WEP40");
        break;
    case WIFI_CIPHER_TYPE_WEP104:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_WEP104");
        break;
    case WIFI_CIPHER_TYPE_TKIP:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_TKIP");
        break;
    case WIFI_CIPHER_TYPE_CCMP:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_CCMP");
        break;
    case WIFI_CIPHER_TYPE_TKIP_CCMP:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_TKIP_CCMP");
        break;
    case WIFI_CIPHER_TYPE_SMS4:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_SMS4");
        break;
    case WIFI_CIPHER_TYPE_GCMP:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_GCMP");
        break;
    case WIFI_CIPHER_TYPE_GCMP256:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_GCMP256");
        break;
    default:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_UNKNOWN");
        break;
    }
}

#ifdef USE_CHANNEL_BTIMAP
static void array_2_channel_bitmap(const uint8_t channel_list[], const uint8_t channel_list_size, wifi_scan_config_t *scan_config) {

    for(uint8_t i = 0; i < channel_list_size; i++) {
        uint8_t channel = channel_list[i];
        scan_config->channel_bitmap.ghz_2_channels |= (1 << channel);
    }
}
#endif /*USE_CHANNEL_BTIMAP*/


void print_scanned_AP(void){

    if(scanner.hasScanned)
    {
        for (int i = 0; i < scanner.maxScannedAP; i++) {
            
            wifi_ap_record_t ap = scanner.Clients[i];

            char *SSID = (char *)ap.ssid;  // This should be a string (char array)
            wifi_auth_mode_t authMode = ap.authmode;
            uint8_t *MAC = ap.bssid;  // This is a MAC address (uint8_t array)
            uint8_t primary_channel = ap.primary;
            uint8_t signalStrength = ap.rssi;
           
        
            ESP_LOGI(TAG, "[*] AP ID: %d", i);
            ESP_LOGI(TAG, "[*] Name: %s", SSID);
            ESP_LOGI(TAG, "[*] MAC: %02X:%02X:%02X:%02X:%02X:%02X", MAC[0], MAC[1], MAC[2], MAC[3], MAC[4], MAC[5]);
            print_auth_mode(authMode);
            ESP_LOGI(TAG,"[*] Signal Strength: %d", signalStrength);
            ESP_LOGI(TAG, "\n\n");

        }
    } 
    else 
    {
        ESP_LOGI(TAG, "[*] No scanned networks available.");
    }
}

wifi_ap_record_t get_AP(uint8_t index){

   wifi_ap_record_t rec = {0};

   if(index < scanner.maxScannedAP || index > 0)
   {
        return scanner.Clients[index];
   }
    
    return rec;
}

uint8_t get_esp_mac_address()
{
    uint8_t MAC[6];
    uint8_t toReturn;

    ESP_ERROR_CHECK(esp_wifi_get_mac(WIFI_MODE_STA, MAC));

    toReturn = *MAC;
    return toReturn;
}
//#FIXME - 
// char format_mac_address(uint8_t *MAC_address)
// {
//     char *format = "%02X:%02X:%02X:%02X:%02X:%02X";
//     char *formattedMAC = "";

//     snprintf(formattedMAC, 18, format, MAC_address[0], MAC_address[1], MAC_address[2], MAC_address[3], MAC_address[4], MAC_address[5]);

//     return *formattedMAC;
// }

uint8_t get_target_mac_address(uint8_t targetID)
{
    wifi_ap_record_t TargetMac = get_AP(targetID);

    uint8_t *MAC;

    MAC = TargetMac.bssid;

    ESP_LOGI(TAG,"MAC of target with ID [%d]: %02X:%02X:%02X:%02X:%02X:%02X", targetID, MAC[0], MAC[1], MAC[2], MAC[3], MAC[4], MAC[5]);

    return *MAC;
}


static void promiscCallBack(void *buff, wifi_promiscuous_pkt_type_t type)
{
    //Packet coming in.
    wifi_promiscuous_pkt_t* packet = (wifi_promiscuous_pkt_t*) buff;
    
    //Extract header + payload.
    wifi_pkt_rx_ctrl_t header = packet->rx_ctrl;
    uint8_t rawPayload = *packet->payload;

    uint8_t *srcMAC   = &rawPayload + 6;
    //uint8_t *dest_mac = $rawPayload;

   ESP_LOGI("promiscCallBack", "Source: %02X:%02X:%02X:%02X:%02X:%02X  -- Destination:", srcMAC[0], srcMAC[1], srcMAC[2], srcMAC[3], srcMAC[4], srcMAC[5]);
    
    switch (type)
    {
        case WIFI_PKT_MGMT:
            // Process management frames
            ESP_LOGI("promiscCallBack", "Received Management Frame");
            
            //Get MAC address - capture both 'from' and 'to'. 
            // 0000	- Association request
            break;

        case WIFI_PKT_CTRL:
            // Process control frames
            ESP_LOGI("promiscCallBack", "Received Control Frame");

            break;

        case WIFI_PKT_DATA:
            // Process data frames
            ESP_LOGI("promiscCallBack", "Received Data Frame");
            break;

        case WIFI_PKT_MISC:
            // Process miscellaneous frames
            ESP_LOGI("promiscCallBack", "Received Miscellaneous Frame");
            break;     

        default:
            ESP_LOGI("promiscCallBack", "Unknown Frame Type");
            break;
    }
}
//Turns on promiscuous mode.
void PromiscOn()
{
    ESP_ERROR_CHECK(esp_wifi_set_promiscuous(true));
    ESP_ERROR_CHECK(esp_wifi_set_promiscuous_rx_cb(promiscCallBack));
    ESP_LOGI(TAG, "promiscuous mode: ON!");
}





/* Initialize Wi-Fi as sta and set scan method */
void wifi_scan(void)
{
    
    // wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    // ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_scan_config_t scan_config = {
        .ssid = 0,
        .bssid = 0,
        .channel = 0,
        .show_hidden = true
    };


    ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, true));
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&scanner.maxScannedAP, scanner.Clients)); // Pass address of number variable
    
    //Only set hasScanned = true if we actually have networks available.
    if(scanner.maxScannedAP > 0)
    {
        scanner.hasScanned = true;

         ESP_LOGI(TAG, "[*] Scan complete. Number of APs found: %d \n", scanner.maxScannedAP);
         print_scanned_AP();

    }
    else 
    {
        scanner.hasScanned = false;

        ESP_LOGI(TAG, "No networks were found!");
    }
}




static void send_packets(uint8_t *packet, size_t packetLen)
{
    ESP_ERROR_CHECK(esp_wifi_80211_tx(WIFI_IF_STA, packet, packetLen, true));
    ESP_LOGI(TAG, "[*] Deauth Packets sent succesfully!");
}

void deauth(uint8_t source_mac, uint8_t destination_mac){

    //TODO - Implement dyynamic insertion of source and mac address.
    uint8_t deauthPacket[26] = {
    /*  0 - 1  */ 0xC0, 0x00,                         // type, subtype c0: deauth (a0: disassociate)
    /*  2 - 3  */ 0x00, 0x00,                         // duration (SDK takes care of that)
    /*  4 - 9  */ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // reciever (target)
    /* 10 - 15 */ 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, // source (ap)
    /* 16 - 21 */ 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, // BSSID (ap)
    /* 22 - 23 */ 0x00, 0x00,                         // fragment & squence number
    /* 24 - 25 */ 0x01, 0x00                          // reason code (1 = unspecified reason)
    };
    size_t packetLen = sizeof(deauthPacket);
    send_packets(deauthPacket,  packetLen);

    //memcpy(&deauthPacket[4], MAC, 6); //source
}






void initialise_wifi(void)
{
    configure_wifi_settings();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

   
    //
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());


    ESP_LOGI(TAG, "[*] WiFi has been initialised\n");

}



