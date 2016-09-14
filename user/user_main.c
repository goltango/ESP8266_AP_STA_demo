/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "esp_common.h"

#define DEMO_STA_SSID "(((WF)))"
#define DEMO_STA_PASSWORD "linknow1987!!!"
#define DEMO_AP_SSID "ESP_WIFI"
#define DEMO_AP_PASSWORD "12345678"


void task1(void *pvParameters)
{
    printf("Hello, welcome to task2!\r\n");
    while (1){
    }
    vTaskDelete(NULL);
}

/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void user_init(void)
{
    uart_div_modify(0, UART_CLK_FREQ / 115200);
    uart_div_modify(1, UART_CLK_FREQ / 115200);
    
    char sofap_mac[6] = {0x16, 0x34, 0x56, 0x78, 0x90, 0xab};
    char sta_mac[6] = {0x12, 0x34, 0x56, 0x78, 0x90, 0xab};
    
    printf("\nSDK version:%s\n", system_get_sdk_version());
    printf("ESP8266 chip ID:0x%x\n", system_get_chip_id());
    
    // Launch the task1
    xTaskCreate(task1, "tsk1", 256, NULL, 2, NULL);
    
    // Set the working mode of ESP8266 as coexistence of station+soft-AP mode.
    wifi_set_opmode(STATIONAP_MODE);
    
    wifi_set_macaddr(SOFTAP_IF, sofap_mac);
    wifi_set_macaddr(STATION_IF, sta_mac);
    
    // Set the SSID and password of the AP. wifi_station_set_config is used to set the AP information 
    // when ESP8266 functions as station. Please be noted that the initialised value of bssid_set in 
    // station_config should be 0, unless the MAC of AP must be specified.
    // wifi_station_connect set the connection of AP.
    struct station_config * configSTA = (struct station_config *)zalloc(sizeof(struct station_config));
    sprintf(configSTA->ssid, DEMO_STA_SSID);
    sprintf(configSTA->password, DEMO_STA_PASSWORD);
    wifi_station_set_config(configSTA);
    free(configSTA);
    wifi_station_connect();
    
 
    // Config ESP8266 functions as soft-AP
    struct softap_config *config = (struct softap_config *)zalloc(sizeof(struct softap_config));
    wifi_softap_get_config(config); // Get soft-AP config first.
    sprintf(config->ssid, DEMO_AP_SSID);
    sprintf(config->password, DEMO_AP_PASSWORD);
    config->authmode = AUTH_WPA_WPA2_PSK;
    config->ssid_len = 0;
    // or its actual SSID length
    config->max_connection = 4;
    wifi_softap_set_config(config); // Set ESP8266 soft-AP config
    free(config);
    
    // Get the station info when ESP8266 functions as soft-AP
    struct station_info * station = wifi_softap_get_station_info();
    while(station){
        os_printf("bssid: %s, ip: %s /n", MAC2STR(station->bssid), IP2STR(&station->ip));
        station = STAILQ_NEXT(station, next);
    }
    wifi_softap_free_station_info();
    // Free it by calling functions
    
    
    // When functions as soft-AP, the default IP address is 192.168.4.1. The IP address is subject to
    // modification by developers, however, before modifying, DHCP server must be closed first.
    // For example, the IP address can be set as 192.168.5.1    
    wifi_softap_dhcps_stop();
    // disable soft-AP DHCP server
    struct ip_info info;
    IP4_ADDR(&info.ip, 192, 168, 5, 1); // set IP
    IP4_ADDR(&info.gw, 192, 168, 5, 1); // set gateway
    IP4_ADDR(&info.netmask, 255, 255, 255, 0); // set netmask
    wifi_set_ip_info(SOFTAP_IF, &info);
    
    // Range of IP address allocated by ESP8266 soft-AP can be set by developers. For example, IP
    // address can range from 192.168.5.100 to 192.168.5.105. Please enable DHCP server when
    // the configuration is completed.
    struct dhcps_lease dhcp_lease;
    IP4_ADDR(&dhcp_lease.start_ip, 192, 168, 5, 100);
    IP4_ADDR(&dhcp_lease.end_ip, 192, 168, 5, 105);
    wifi_softap_set_dhcps_lease(&dhcp_lease);
    wifi_softap_dhcps_start(); // enable soft-AP DHCP server
}

