// Basic Config Definiion

#pragma once
#include <ArduinoJson.h>

/*---  Example JSON ---
{ 
"StaConfig" :
  [  { "ssid" : "ssid1",      "pw" : "pw1"} , 
     { "ssid" : "ssid2",      "pw" : "pw2"}  ] ,
"SoftAPConfig" : { "ssid" : "APssid",       "pw": "passphrase"} ,
"ServerConfig"  :  { "hostname": "hostname"} ,
"ApplConfig"    :  { "door_up" :  2650, 
                     "door_mid" : 1300,
                     "door_down" : 390 }
---*/

//********* Default values if "config.json" not found or parameter sets missing

//**** No WiFi-Data by Default ==> Start Soft-ACessPoint....
#define DEF_STA_SSID_0        ""            // First SSID - try to connect...
#define DEF_STA_PASS_0        ""            // First Password
#define DEF_STA_SSID_1        ""            // Second SSID - try to connect...
#define DEF_STA_PASS_1        ""            // Second Password
//**** Default hostname...
#define DEF_HOSTNAME          "garage"      // default hostname
//**** SoftAP data...
#define DEF_SOFT_AP_SSID      "Garage_AP"   // Default SoftAP SSID
#define DEF_SOFT_AP_PASS      "garage_pass" // Default SoftAP PassPhrase
//**** Default ADC-Values to determin door_level...
#define DEF_DOOR_UP           2650
#define DEF_DOOR_MID          1300
#define DEF_DOOR_DOWN         390

//*****************************************************

struct StaConfig {
  char StaSsid[32];            //SSID for STA-Mode
  char StaPassphrase[64];      //PassPhrase for STA-Mode
};

// Stores the configuration of the AP
struct SoftApConfig {
  char SoftApSsid[32];            //SSID for SoftAP
  char SoftApPasspphrase[64];    //PassPhrase for SoftAP
};

// Stores the server configuration
struct ServerConfig {
  char hostname[32];      // hostname of Server (if supporte by AP)
};
// Stores the application specific settings
struct ApplConfig {
  int door_up   ;   // from here on "OPEN"
  int door_mid  ;   // Between door_open...door_mid ==> "NOT OPEN"
  int door_down ;   // Between door_mid...door_close ==> "NOT CLOSED"
                          // Below door_close ==> "CLOSED"
};

// Stores the complete configuration
struct ConfigStruct {
  static const int maxStaConfigs = 2;
  StaConfig StaCfg[maxStaConfigs];
  int curStaConfigs = 0;
  ServerConfig ServerCfg;
  SoftApConfig  SoftApCfg;
  ApplConfig  ApplCfg;
};

//**** Function prototypes *******************************************
// *** Convert Config from/to JSON 
void convertToJson(const ConfigStruct &src, JsonVariant dst);
void convertFromJson(JsonVariantConst src, ConfigStruct &dst);

// *** Load and Save Config-File
bool loadConfigFile(const char *filename, ConfigStruct &config);
bool saveConfigFile(const char *filename, const ConfigStruct &config);
void printFile(const char *filename);
void setDefaultConfig(ConfigStruct &config);
