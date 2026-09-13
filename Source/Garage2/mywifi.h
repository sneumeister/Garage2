//************** Alles für WiFi: STA + AP ******************
//**********************************************************

#pragma once

#define DEBUGINFO 0
#include "debug.h"

#include <WiFi.h>
#include "Config.h"

#define MYWIFI_MAX_TRIALS 25      //25    // Max number of checks...
#define MYWIFI_TRIAL_INTERVAL 500     // Milisecs. between Status checks...
#define MAX_FAILS_FOR_AP_START 5  //20   // After this number of failes Connecting-cycles start local AP
#define SOFTAP_STOP_GRACE_MS (5UL * 60UL * 1000UL)  // after STA connect, keep SoftAP this long if clients are connected

void        WifiInit(const char* hostname, const char* ap_SSID);                // Setting the Wifi-Mode; run once from setup()...........
bool        WifiStartAP(const SoftApConfig *SoftAPCfg, const bool SwitchApUp ); // Start/Stop SoftAP... function is called from WifiReConnect().. SwitchApUp==true for starting AP...
wl_status_t WifiReConnect(const StaConfig StaCfg[], const int maxStaCfgs, const char *hostname , const SoftApConfig* SoftAPCfg ) ;  // Called from loop() whenever STA-connection lost... if reconnect fails => Start AP....
//void        WifiStopAP();
