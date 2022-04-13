// Handle Config-File
// Based on a sample file from the book "Mastering ArduinoJson"
// https://arduinojson.org/book/
// Copyright 2017-2021 Benoit Blanchon

#pragma once

#define DEBUGINFO 0

#include <SPIFFS.h>
#include "debug.h"
#include "Config.h"

/* File format:
{ 
"StaConfig" :
  [  { "ssid" : "ssid1",      "pw" : "pw1"} , 
     { "ssid" : "ssid2",      "pw" : "pw2"}  ] ,
"SoftAPConfig" : { "ssid" : "APssid",       "pw": "passphrase"} ,
"ServerConfig"  :  { "hostname": "hostname"} ,
"ApplConfig"    :  { "door_up" :  2650, 
                     "door_mid" : 1300,
                     "door_down" : 390 }
}
---------------------*/

//---------- Setting Default values ------------------------------------------------
void setDefaultConfig(ConfigStruct &config) {
    strcpy(config.StaCfg[0].StaSsid, DEF_STA_SSID_0);
    strcpy(config.StaCfg[0].StaPassphrase, DEF_STA_PASS_0);
    strcpy(config.StaCfg[1].StaSsid, DEF_STA_SSID_1);
    strcpy(config.StaCfg[1].StaPassphrase, DEF_STA_PASS_1);
    config.curStaConfigs = 0;
    strcpy(config.SoftApCfg.SoftApSsid, DEF_SOFT_AP_SSID);
    strcpy(config.SoftApCfg.SoftApPasspphrase, DEF_SOFT_AP_PASS);
    strcpy(config.ServerCfg.hostname, DEF_HOSTNAME);
    config.ApplCfg.door_up   = DEF_DOOR_UP;
    config.ApplCfg.door_mid  = DEF_DOOR_MID;
    config.ApplCfg.door_down = DEF_DOOR_DOWN;
}

//---------- Start of StaConfig-Parse ----------------------------------------------
// Converts an StaConfig to JSON
// (automatically called by `aps.add(src.StaCfg[i])`)
void convertToJson(const StaConfig &src, JsonVariant dst) {
  dst["ssid"] = src.StaSsid;
  dst["pw"]   = src.StaPassphrase;
}

// Extracts an StaConfig from JSON
// (automatically called by `dst.accessPoint[dst.accessPoints] = ap`)
void convertFromJson(JsonVariantConst src, StaConfig &dst) {
  strlcpy(dst.StaSsid, src["ssid"] | "", sizeof(dst.StaSsid));
  strlcpy(dst.StaPassphrase, src["pw"] | "", sizeof(dst.StaPassphrase));
}
//---------- End of StaConfig-Parse
//---------- Start of SoftApConfig-Parse
// Extracts a ServerConfig to JSON
// (automatically called by `dst["SoftApConfig"] = src.SoftApCfg`)
void convertToJson(const SoftApConfig &src, JsonVariant dst) {
  dst["ssid"] = src.SoftApSsid;
  dst["pw"]   = src.SoftApPasspphrase;
}
// Convert a ServerConfig from JSON
// (automatically called by `dst.server = src["server"]`)
void convertFromJson(JsonVariantConst src, SoftApConfig &dst) {
  strlcpy(dst.SoftApSsid, src["ssid"] | "", sizeof(dst.SoftApSsid));
  strlcpy(dst.SoftApPasspphrase, src["pw"] | "", sizeof(dst.SoftApPasspphrase));
}
//---------- End of SoftApConfig-Parse
//---------- Start of ServerConfig-Parse
// Extracts a ServerConfig to JSON
// (automatically called by `dst["ServerConfig"] = src.ServerCfg`)
void convertToJson(const ServerConfig &src, JsonVariant dst) {
  dst["hostname"] = src.hostname;
}
// Convert a ServerConfig from JSON
// (automatically called by `dst.server = src["server"]`)
void convertFromJson(JsonVariantConst src, ServerConfig &dst) {
  strlcpy(dst.hostname, src["hostname"] | "", sizeof(dst.hostname));
}
//---------- End of ServerpConfig-Parse
//---------- Start of ApplConfig-Parse
// Extracts a ServerConfig to JSON
// (automatically called by `dst["ApplConfig"] = src.ApplCfg`)
void convertToJson(const ApplConfig &src, JsonVariant dst) {
  dst["door_up"]    =  src.door_up;
  dst["door_mid"]   =  src.door_mid;
  dst["door_down"]  =  src.door_down;
}
// Convert a ServerConfig from JSON
// (automatically called by `dst.server = src["server"]`)
void convertFromJson(JsonVariantConst src, ApplConfig &dst) {
  dst.door_up   =  src["door_up"].as<int>();
  dst.door_mid  =  src["door_mid"].as<int>();
  dst.door_down =  src["door_down"].as<int>();
}
//---------- End of ApplConfig-Parse
//-------- Start of Parse Config overall   
// Converts a Config to JSON
// (automatically called by `config = doc.as<ConfigStruct>()`)
void convertToJson(const ConfigStruct &src, JsonVariant dst) {
  // Add "SoftApConfig" object
  dst["SoftApConfig"] = src.SoftApCfg;
  // Add "ServerConfig" object
  dst["ServerConfig"] = src.ServerCfg;
  // Add "ApplConfig" object
  dst["ApplConfig"] = src.ApplCfg;

  // Add "StaCfgs"s array
  JsonArray aps = dst.createNestedArray("StaConfig");

  // Add each acces point in the array
  for (int i = 0; i < src.curStaConfigs; i++)
    aps.add(src.StaCfg[i]);
}
// Extracts a Config from JSON
// (automatically called by `doc.set(config)`)
void convertFromJson(JsonVariantConst src, ConfigStruct &dst) {
  // Read "SoftApConfig" object
  dst.SoftApCfg = src["SoftApConfig"];
  // Read "ServerConfig" object
  dst.ServerCfg= src["ServerConfig"];
  // Read "ApplConfig" object
  dst.ApplCfg = src["ApplConfig"];

  // Get a reference to the access_points array
  JsonArrayConst aps = src["StaConfig"];
  // Extract each access points
  dst.curStaConfigs = 0;
  for (JsonVariantConst ap : aps) {
    // Load the AP
    dst.StaCfg[dst.curStaConfigs] = ap;

    // Increment AP count
    dst.curStaConfigs++;

    // Max reach?
    if (dst.curStaConfigs >= ConfigStruct::maxStaConfigs)
      break;
   }
}
//-------- End of Parse Config overall  --------------------------------------------
//******************************************************
//**** Loads the configuration from a file
bool loadConfigFile(const char *filename, ConfigStruct &config) {
  // Open file for reading
  bool ret=false;
  File file = SPIFFS.open(filename, FILE_READ);
  if (!file) {
    DEBUG_PRINTS("Failed to open config file"); DEBUG_PRINTLN();
    ret=false;
  } else {
    // Allocate the JsonDocument
    StaticJsonDocument<512> doc;
    // Parse the JSON object in the file
    DeserializationError err = deserializeJson(doc, file);
    // This may fail if the JSON is invalid
    if (err) {
      DEBUG_PRINTS("Failed to deserialize configuration: ");
      DEBUG_PRINT("", err.f_str());
      DEBUG_PRINTLN();
      ret=false;
    } else {
      // Extract config from the JSON document
      config = doc.as<ConfigStruct>();
      ret=true;
    }
  }
return ret;
}
//******************************************************
//**** Saves the configuration to a file
bool saveConfigFile(const char *filename, const ConfigStruct &config) {
  // Open file for writing
  bool ret=false;
  File file = SPIFFS.open(filename, FILE_WRITE);
  if (!file) {
    DEBUG_PRINTS("Failed to create config file."); DEBUG_PRINTLN();
    ret=false;
  } else {
    // Allocate the JsonDocument
    StaticJsonDocument<512> doc;
    // Fill JSON document from config
    doc.set(config);
    // Serialize JSON to file
    bool success = serializeJsonPretty(doc, file) > 0;
    if (!success) {
      DEBUG_PRINTS("Failed to serialize configuration"); DEBUG_PRINTLN();
      ret=false;
    } else
      ret=true;
  }
  return ret;
}
//******************************************************
//**** Prints the content of a file to the Serial on DEBUG
void printFile(const char *filename) {
  // Open file for reading
  File file = SPIFFS.open(filename, FILE_READ);
  if (!file) {
    DEBUG_PRINTS("Failed to open config file"); DEBUG_PRINTLN();
  } else {
  // Extract each by one by one
    while (file.available()) {
      Serial.print(""); Serial.print((char)file.read());
//      DEBUG_PRINT("", (char)file.read());
    }
    DEBUG_PRINTLN();
  }
}
