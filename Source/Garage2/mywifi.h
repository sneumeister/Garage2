//************** Alles für WiFi: STA + AP ******************
//**********************************************************

#pragma once

#include <WiFi.h>
#include "debug.h"

#define DEBUGINFO 0
#include "config.h"

#define MYWIFI_MAX_TRIALS 25      //25    // Max number of checks...
#define MYWIFI_TRIAL_INTERVAL 500     // Milisecs. between Status checks...
#define MAX_FAILS_FOR_AP_START 5  //20   // After this number of failes Connecting-cycles start local AP

void        WifiInit(const char* hostname, const char* ap_SSID);                // Setting the Wifi-Mode; run once from setup()...........
bool        WifiStartAP(const SoftApConfig *SoftAPCfg, const bool SwitchApUp ); // Start/Stop SoftAP... function is called from WifiReConnect().. SwitchApUp==true for starting AP...
wl_status_t WifiReConnect(const StaConfig StaCfg[], const int maxStaCfgs, const char *hostname , const SoftApConfig* SoftAPCfg ) ;  // Called from loop() whenever STA-connection lost... if reconnect fails => Start AP....
//void        WifiStopAP();
