//************** Alles für WiFi: STA + AP ******************
//**********************************************************

#include "myArduinoOTA.h"

//**********************************************************
//********** ArduinoOTA definitions...
void setupArduinoOTA(const char* cfg_hostname){
  const char* OtaPasswordHash = PASSWORD_HASH;
  
  #ifdef AOTA_PORT
    ArduinoOTA.setPort(3232);            // Port defaults to 3232
  #endif
  
  ArduinoOTA.setHostname(cfg_hostname);  // Hostname defaults to esp3232-[MAC]
  // ArduinoOTA.setPassword("admin"); // No authentication by default
  // Password can be set with it's md5 value as well
  // MD5(Garagensteuerung!) = f8695e6ee86ea0b76ebbbe9023f6ae70
  ArduinoOTA.setPasswordHash(OtaPasswordHash);
  ArduinoOTA
    .onStart([]() {
      String OTAtype;
      if (ArduinoOTA.getCommand() == U_FLASH)
        OTAtype = "sketch";
      else // U_SPIFFS
        OTAtype = "filesystem";
        SPIFFS.end();
      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      DEBUG_PRINT("Start updating ", OTAtype); DEBUG_PRINTLN();
    })
    .onEnd([]() {
      DEBUG_PRINTS("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      DEBUG_PRINT("Progress: ", (progress / (total / 100)) ); DEBUG_PRINTLN();
      })
    .onError([](ota_error_t error) {
      DEBUG_PRINT("Error:", error );
      if (error == OTA_AUTH_ERROR) { DEBUG_PRINTS("Auth Failed"); DEBUG_PRINTLN(); }
      else if (error == OTA_BEGIN_ERROR) { DEBUG_PRINTS("Begin Failed"); DEBUG_PRINTLN(); }
      else if (error == OTA_CONNECT_ERROR) { DEBUG_PRINTS("Connect Failed"); DEBUG_PRINTLN(); }
      else if (error == OTA_RECEIVE_ERROR) { DEBUG_PRINTS("Receive Failed"); DEBUG_PRINTLN(); }
      else if (error == OTA_END_ERROR) { DEBUG_PRINTS("End Failed"); DEBUG_PRINTLN();}
    });
  ArduinoOTA.begin();
   DEBUG_PRINTS("Upload finished."); DEBUG_PRINTLN();
}
//**********************************************************
//********** ArduinoOTA handler...
void loopArduinoOTA(){
  ArduinoOTA.handle();  //********
}
