//************* actual fuctions for WiFi: STA + AP***********
// ESP32-Arduino-WiFi refernce: 
//      https://docs.espressif.com/projects/arduino-esp32/en/latest/api/wifi.html
//

#pragma once
#include "mywifi.h"

//******************************************************************************************
// Setting the Wifi-Mode; run once from setup()...........
void WifiInit(const char* hostname, const char* ap_SSID){  
    WiFi.disconnect();
    WiFi.persistent(false);
    WiFi.mode(WIFI_MODE_APSTA);
    WiFi.softAPsetHostname(hostname);
    WiFi.setHostname(hostname);
    WiFi.softAPdisconnect(true);
    waitMs(1000);
    DEBUG_PRINTS("mywifi: setting WiFi.mode(WIFI_MODE_APSTA)."); DEBUG_PRINTLN();
}

//******************************************************************************************
// Starting SoftAP... enabling connection when Client connect fails.
bool  WifiStartAP(const SoftApConfig *SoftApCfg, const bool SwitchApUp ){
  static bool ApUp=false;  // is AP up.. ?
  bool  ret=false;
  
  if (SwitchApUp && !ApUp && WiFi.status() !=  WL_CONNECTED ) {
    //  WiFi.softAP(ssid, psk, channel, hidden, max_connection)
    WiFi.softAPsetHostname(WiFi.getHostname());
    ApUp=WiFi.softAP(SoftApCfg->SoftApSsid, SoftApCfg->SoftApPasspphrase ,1 , false, 2  );
    ret=ApUp;
    DEBUG_PRINT("mywifi: WiFiStartAP(softAPSSID): AP-SSID='", WiFi.softAPSSID() ); DEBUG_PRINTS("'"); DEBUG_PRINTLN();
    DEBUG_PRINT("mywifi: WiFiStartAP(config): AP-SSID='", SoftApCfg->SoftApSsid ); DEBUG_PRINTS("'"); DEBUG_PRINTLN();
    DEBUG_PRINT("mywifi: WiFiStartAP(): AP-PW='", (SoftApCfg->SoftApPasspphrase));DEBUG_PRINTS("'"); DEBUG_PRINTLN();
    DEBUG_PRINT("mywifi: WiFiStartAP(): AP-IP='", WiFi.softAPIP() );DEBUG_PRINTS("'"); DEBUG_PRINTLN();
  } 
  else if (!SwitchApUp && ApUp && !WiFi.softAPgetStationNum() ) {
    ret=WiFi.softAPdisconnect(true);
    DEBUG_PRINT("mywifi: SoftAP disconnected:", ret);DEBUG_PRINTLN();
  } 
  else if (!SwitchApUp && ApUp && WiFi.softAPgetStationNum() ) {
    // At least one connection to AP..... do not siconnect....
    ret=false;
    DEBUG_PRINT("mywifi: SoftAP must not disconnect: Client conneted: ", WiFi.softAPgetStationNum() );DEBUG_PRINTLN();
  } 
  else {
    ret=false;
  }
  if (ApUp) {
//    DEBUG_PRINT("mywifi: Connected clients: ", WiFi.softAPgetStationNum()  );DEBUG_PRINTLN();
  }
  
  return(ret);
//  DEBUG_PRINT("mywifi: WiFiStartAP(): AP-SSID='",(SoftApCfg->SoftApSsid));DEBUG_PRINTS("'");DEBUG_PRINTLN();
//  DEBUG_PRINT("mywifi: WiFiStartAP(): AP-PW='", (SoftApCfg->SoftApPasspphrase));DEBUG_PRINTS("'");DEBUG_PRINTLN();
}

//******************************************************************************************
// Called from loop() whenever STA-connection lost... if reconnect fails => Start AP....
wl_status_t WifiReConnect(const StaConfig StaCfg[], const int maxStaCfgs, const char *hostname , const SoftApConfig *SoftAPCfg) {
  const int   WLmaxCount=MYWIFI_MAX_TRIALS;
  const int   CheckInterval=MYWIFI_TRIAL_INTERVAL;
  wl_status_t ret=WL_CONNECT_FAILED;
  static int  curStaCfg=-1;
  static int  failedCycles=MAX_FAILS_FOR_AP_START;
  static  bool  runonce=false;

  
  if (strcmp(WiFi.getHostname(), hostname)) {
    WiFi.setHostname(hostname);
    DEBUG_PRINT("mywifi: Setting Hostname: ", hostname); DEBUG_PRINTLN();
  }

  if (maxStaCfgs>0 && !WiFi.softAPgetStationNum() ) {
    curStaCfg++;
    if (curStaCfg>=maxStaCfgs) { curStaCfg=0; }

    DEBUG_PRINT("Setting STA index: ", curStaCfg); DEBUG_PRINTLN();

    if (StaCfg[curStaCfg].StaSsid[0]==NULL) {
      DEBUG_PRINTS("Connecting.... No WiFi Name, abort!" ); DEBUG_PRINTLN();
      ret=WL_NO_SSID_AVAIL; }
    else {
      WiFi.begin(StaCfg[curStaCfg].StaSsid, StaCfg[curStaCfg].StaPassphrase);
  
      DEBUG_PRINT("Connecting to WiFi '", StaCfg[curStaCfg].StaSsid );
      DEBUG_PRINTS("':");
      if ( WiFi.status() !=  WL_CONNECTED ) {
        int WLcount = 0;
        while (WiFi.status() != WL_CONNECTED && WLcount < WLmaxCount ) {
          waitMs( CheckInterval );
          DEBUG_PRINTS(".");
          ++WLcount;
        }
        if (WiFi.status() == WL_CONNECTED) {
          DEBUG_PRINTLN();
          Serial.print("Locale IP: "); Serial.print( WiFi.localIP() ); Serial.print("\n");
          //DEBUG_PRINT("Locale IP: ", WiFi.localIP()); DEBUG_PRINTLN();
          //DEBUG_PRINT("RSSI: ", WiFi.RSSI()); DEBUG_PRINTLN();
        } else  { DEBUG_PRINTS(". FAILED!"); DEBUG_PRINTLN(); }
      }
      ret=WiFi.status(); 
    }
  }
  if ( ret!= WL_CONNECTED ) {
    if ( failedCycles>0 ) { 
      failedCycles--;
      DEBUG_PRINT("mywifi: failedCycles=", failedCycles); DEBUG_PRINTLN();  }
  } else {
    failedCycles=MAX_FAILS_FOR_AP_START;
    WifiStartAP(SoftAPCfg, false);    // false == Stopp AP
  }
//  DEBUG_PRINT("mywifi: failedCycles=", failedCycles); DEBUG_PRINTLN();
  if (!failedCycles) {
 //*---------------------------- AP function ------------------
    WifiStartAP(SoftAPCfg, true);    // true == Start AP
  }
     
  return(ret);
}
