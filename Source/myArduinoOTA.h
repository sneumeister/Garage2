//*********** ArduinoOTA - Over-The-Air updates**********
//*******************************************************

#pragma once

#include <ArduinoOTA.h>
#include "config.h"
#include <SPIFFS.h>

#define DEBUGINFO 1           // Compile DEBUG clauses....
#include "debug.h"            // Switch on/of Debug Info via 'Serial'

//#define AOTA_PORT 3232   // Default-Port: 3232

// ArduinoOTA.setPassword("admin"); // No authentication by default
// Password can be set with it's md5 value as well
// MD5(Garagensteuerung!) = f8695e6ee86ea0b76ebbbe9023f6ae70
#define PASSWORD_HASH  "f8695e6ee86ea0b76ebbbe9023f6ae70";

//*******************************************************
//*** function proto-types.
void setupArduinoOTA(const char* hostname);
void loopArduinoOTA();
