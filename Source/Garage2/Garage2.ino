// This is a sample file from the book "Mastering ArduinoJson"
// https://arduinojson.org/book/
// Copyright 2017-2021 Benoit Blanchon
//
// This example shows how to store your project configuration in a JSON file.
// The file is stored in SPIFFS, so you need something compatible (like an
// ESP8266) to run this code.

//*** Version Informationen ********
#include "_sketchversion.h";
//*** Allgemeine Includes **********
#include <SPIFFS.h>
//*** Load Wi-Fi library
//#include <WiFi.h>
#include <string.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
//#include "ESPAsyncDNSServer.h";

//*** Meine Includes **********
#define DEBUGINFO 1           // Compile DEBUG clauses....

#include "debug.h"            // Switch on/of Debug Info via 'Serial'
#include "littleHelpers.h"    // Little Helper functions
#include "Config.h"           // Configuration Struct and JSON De-/Serialize functions...
#include "mywifi.h"           // Everything with Wifi connections: STA + AP
#include "hardwareRelated.h"  // Everything related to hardware GPIO , Pins..
#include "myArduinoOTA.h"     // ARduinoOTA for Over-The-Air upadtes from ArduinoIDE...

//*** Globale Variablen und Const-anten
//**** Pfad zur Config-Datei
const char* filename = "/config.json"; 
const char* web_path = "/";

ConfigStruct config;                    // Konfigurations-Struct

//*** Seeting up the Server ************
AsyncWebServer server(80);

//*** DNS server
const byte DNS_PORT = 53;
DNSServer dnsServer;
//AsyncDNSServer dnsServer;


//AsyncWebSocket ws("/ws");             // access at ws://[esp ip]/ws
//AsyncEventSource events("/events");   // event source (Server-Sent events)

//******************************************************
//**** Handle POST request to save settings...
void handlerSettingsPOST(AsyncWebServerRequest *request){
  const char*  PARAM_ACTION_ID="action";
  const char*  ACTION_STA1     ="setstaclient1";
  const char*  ACTION_STA1_SSID="stassid1";
  const char*  ACTION_STA1_PWD ="stapasswd1";
  const char*  ACTION_STA2     ="setstaclient2";
  const char*  ACTION_STA2_SSID="stassid2";
  const char*  ACTION_STA2_PWD ="stapasswd2";
  const char*  ACTION_ADC      ="set_door_values";
  const char*  ACTION_ADC_DWN  ="door_down";
  const char*  ACTION_ADC_MID  ="door_mid";
  const char*  ACTION_ADC_UP   ="door_up";
  const char*  ACTION_HOST       ="sethostname";
  const char*  ACTION_HOST_HST   ="hostname";
 
  DEBUG_PRINTS("Handler POST handling: ");
//*** Wenn's einen "action=" parameter gibt ************************************************************
  if (request->hasParam( PARAM_ACTION_ID , true)) {
    DEBUG_PRINT("action='", request->getParam(PARAM_ACTION_ID, true)->value()) ; DEBUG_PRINTS("'"); DEBUG_PRINTLN();
//***** Wenn ACTION_STA1="setstaclient1" ==> Setze STA[0]: SSID + PASSPHRASE ***************************
    if ( strcmp( request->getParam(PARAM_ACTION_ID, true)->value().c_str() , ACTION_STA1 )==0){
      DEBUG_PRINT("==> '", ACTION_STA1 ); DEBUG_PRINTS("'"); DEBUG_PRINTLN();

//***** Wenn ACTION_STA1_SSID="stassid1" ==> Setze STA[0]: SSID ****************************************
      if (request->hasParam( ACTION_STA1_SSID , true)) {
        DEBUG_PRINT("==> ", ACTION_STA1_SSID ); DEBUG_PRINT("=", request->getParam(ACTION_STA1_SSID, true)->value() ); DEBUG_PRINTLN();
        strlcpy(config.StaCfg[0].StaSsid, request->getParam(ACTION_STA1_SSID, true)->value().c_str(), sizeof(config.StaCfg[0].StaSsid));
      }
//***** Wenn ACTION_STA1_PWD ="stapasswd1" ==> Setze STA[0]: PASSPHRASE ********************************
      if (request->hasParam( ACTION_STA1_PWD , true)) {
        DEBUG_PRINT("==> ", ACTION_STA1_PWD ); DEBUG_PRINT("=", request->getParam(ACTION_STA1_PWD, true)->value() ); DEBUG_PRINTLN();
        strlcpy(config.StaCfg[0].StaPassphrase, request->getParam(ACTION_STA1_PWD, true)->value().c_str(), sizeof(config.StaCfg[0].StaPassphrase));
      }
    }
//***** Wenn ACTION_STA2="setstaclient2" ==> Setze STA[0]: SSID + PASSPHRASE ***************************
    else if ( strcmp( request->getParam(PARAM_ACTION_ID, true)->value().c_str() , ACTION_STA2 )==0){
      DEBUG_PRINT("==> '", ACTION_STA2 ); DEBUG_PRINTS("'"); DEBUG_PRINTLN();
//***** Wenn ACTION_STA2_SSID="stassid2" ==> Setze STA[1]: SSID ****************************************
      if (request->hasParam( ACTION_STA2_SSID , true)) {
        DEBUG_PRINT("==> ", ACTION_STA2_SSID ); DEBUG_PRINT("=", request->getParam(ACTION_STA2_SSID, true)->value() ); DEBUG_PRINTLN();
        strlcpy(config.StaCfg[1].StaSsid, request->getParam(ACTION_STA2_SSID, true)->value().c_str(), sizeof(config.StaCfg[1].StaSsid));
      }
//***** Wenn ACTION_STA2_PWD ="stapasswd2" ==> Setze STA[1]: PASSPHRASE ********************************
      if (request->hasParam( ACTION_STA2_PWD , true)) {
        DEBUG_PRINT("==> ", ACTION_STA2_PWD ); DEBUG_PRINT("=", request->getParam(ACTION_STA2_PWD, true)->value() ); DEBUG_PRINTLN();
        strlcpy(config.StaCfg[1].StaPassphrase, request->getParam(ACTION_STA2_PWD, true)->value().c_str(), sizeof(config.StaCfg[1].StaPassphrase));
      }
    }
//***** Setze ADC Werte ***************************
//***** Wenn ACTION_ADC="set_door_values" ==> Setze alle 3 Door Values *********************************
    else if ( strcmp( request->getParam(PARAM_ACTION_ID, true)->value().c_str() , ACTION_ADC )==0){
      DEBUG_PRINT("==> '", ACTION_ADC ); DEBUG_PRINTS("'"); DEBUG_PRINTLN();
//***** Wenn ACTION_ADC_DWN="door_down" ==> Setzte Door_Down *******************************************
      if (request->hasParam( ACTION_ADC_UP , true)) {
        DEBUG_PRINT("==> ", ACTION_ADC_UP ); DEBUG_PRINT("=", request->getParam(ACTION_ADC_UP , true)->value() ); DEBUG_PRINTLN();
        config.ApplCfg.door_up = atoi( request->getParam(ACTION_ADC_UP, true)->value().c_str() );
      }
//***** Wenn ACTION_ADC_MID="door_mid" ==> Setzte Door_Mid ********************************************
      if (request->hasParam( ACTION_ADC_MID , true)) {
        DEBUG_PRINT("==> ", ACTION_ADC_MID ); DEBUG_PRINT("=", request->getParam(ACTION_ADC_MID , true)->value() ); DEBUG_PRINTLN();
        config.ApplCfg.door_mid = atoi( request->getParam(ACTION_ADC_MID, true)->value().c_str() );
      }
//***** Wenn ACTION_ADC_DWN="door_down" ==> Setzte Door_Down ********************************************
      if (request->hasParam( ACTION_ADC_DWN , true)) {
        DEBUG_PRINT("==> ", ACTION_ADC_DWN ); DEBUG_PRINT("=", request->getParam(ACTION_ADC_DWN , true)->value() ); DEBUG_PRINTLN();
        config.ApplCfg.door_down = atoi( request->getParam(ACTION_ADC_DWN, true)->value().c_str() );
      }
    }
//***** Setze Hostname ***************************
//***** Wenn ACTION_HOST="sethostname" ==> Setze hostname **********************************************
    else if ( strcmp( request->getParam(PARAM_ACTION_ID, true)->value().c_str() , ACTION_HOST )==0){
      DEBUG_PRINT("==> '", ACTION_HOST ); DEBUG_PRINTS("'"); DEBUG_PRINTLN();
//***** Wenn ACTION_HOST_HST="hostname" ==> Setze hostname *********************************************
      if (request->hasParam( ACTION_HOST_HST , true)) {
        DEBUG_PRINT("==> ", ACTION_HOST_HST ); DEBUG_PRINT("=", request->getParam(ACTION_HOST_HST , true)->value() ); DEBUG_PRINTLN();
        strlcpy(config.ServerCfg.hostname, request->getParam(ACTION_HOST_HST, true)->value().c_str(), sizeof(config.ServerCfg.hostname));
      }
    }
//***** Irgend ein "action=" wurde gefunden ==> Speichere config im File ***************************
  saveConfigFile(filename, config); 
  #ifdef DEBUGINFO
    printFile(filename);
  #endif
//***** Kein "action=" gefunden ==> ignorieren ***************************
  } else {
    DEBUG_PRINTS(" No valid action! "); DEBUG_PRINTLN();
  }
//***** Lade "/config.html" einfach erneut ***************************  
  request->redirect("/config.html");
}

//***************** Setup *************************
void setup() {
  // Initialize serial port
  Serial.begin(115200);
  
  #ifdef DEBUGINFO
    Serial.setDebugOutput(true); // Debug Output for WLAN on Serial Interface.
  #endif
  
  if(!SPIFFS.begin(true)){ DEBUG_PRINTS("Error while mounting SPIFFS!\n"); }
  else { DEBUG_PRINTS("OK.\n"); }

  // Load configuration
  DEBUG_PRINTS("Load config..."); DEBUG_PRINTLN();
  bool loaded = loadConfigFile(filename, config);

  if (!loaded) {
    DEBUG_PRINTS("Using default config"); DEBUG_PRINTLN();
    setDefaultConfig(config);
    // Save configuration
    DEBUG_PRINTS("Save Config File."); DEBUG_PRINTLN();
    saveConfigFile(filename, config);
  }
  #if DEBUGINFO
    DEBUG_PRINTS("Print Config File...."); DEBUG_PRINTLN();
    printFile(filename);
  #endif

  WifiInit(config.ServerCfg.hostname, config.SoftApCfg.SoftApSsid);   // Set Wifi-Mode
  //*** Handler definitions ***************************************
  //**** Load HTML-Code handlers ***********************************
  server.rewrite("/", "/index.html");
  server.rewrite("/config", "/config.html");
  server.rewrite("/generate_204", "/config.html").setFilter(ON_AP_FILTER);
  server.rewrite("/fwlink", "/config.html").setFilter(ON_AP_FILTER);
  
//**********************************************************************************
//**** Hello world!
  server.on("/hello", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "Hello World"); });

//**********************************************************************************
//**** Output Version and Compile Date/Time

server.on("/version", HTTP_GET, [](AsyncWebServerRequest *request){
      DEBUG_PRINTS("webserver: "); DEBUG_PRINTS(SKETCHNAME); DEBUG_PRINTS(" "); DEBUG_PRINTS(SKETCHVERSION); DEBUG_PRINTS(" ("); DEBUG_PRINTS(SKETCHDATE); DEBUG_PRINTS(")");
      DEBUG_PRINTS("Compiled "); DEBUG_PRINTS(SKETCHCOMPILE); DEBUG_PRINTLN();
      request->send(200, "text/plain", String (SKETCHNAME)
                                      +String(" ")
                                      +String(SKETCHVERSION)
                                      +String(" (")
                                      +String(SKETCHDATE)
                                      +String(")\nCompiled ")
                                      +String(SKETCHCOMPILE)
                                      +String("\n")               ); });



//**********************************************************************************
//**** Read Value of ADC
    server.on("/raw_adc", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/plain", ( String(read_door_adc() ) /*  String(random_number(100 , 8000)) */
        )); });

//**********************************************************************************
//**** Zeige aktuelle RSSI
    server.on("/raw_rssi", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", (String(WiFi.RSSI()))); });

//**********************************************************************************
//**** Send door position
  server.on("/doorlevel", HTTP_GET, [](AsyncWebServerRequest *request){
    signalLed("...");  //*** 3x short...
    DEBUG_PRINT("Web: Door level requested: ", door_level( read_door_adc() , config.ApplCfg   ) )  ; DEBUG_PRINTLN();
    request->send(200, "text/plain", String((door_level( read_door_adc() , config.ApplCfg   ) ) ) ); });

//**********************************************************************************
//**** Push button to open door.....
  server.on("/push_the_button", HTTP_POST, [](AsyncWebServerRequest *request){
    signalLed("- - -");  // Signal: 3x lang...
    if (request->hasParam("action", true)) {
      if ( strcmp( request->getParam("action", true)->value().c_str() , "push" )==0){
        push_the_button();
        request->send(202, "text/plain", F("Done."));  
        DEBUG_PRINTS("Button: pushed!"); DEBUG_PRINTLN();
      }
    } else {
        request->send(400, "text/plain", F("Error: wrong magic word!"));  
        DEBUG_PRINTS("Button: wrong magic word!\n");
    }
  });


//**********************************************************************************
//**** "index.html" handler
  server.serveStatic("/index.html", SPIFFS, "/index.html").setTemplateProcessor(
          [](const String &var){  String ret=var;
                                  if(var == "GARAGELEVELICON") {
                                    String num = (String(1+door_level(read_door_adc(), config.ApplCfg)));
                                    String ret = "<img width=\"150\" height=\"auto\" src=\""+String(web_path)+"/garage"+num+".png\"/>";
                                  }  return( ret ); });

//**********************************************************************************
//**** "config.html" handler
    server.serveStatic("/config.html", SPIFFS, "/config.html").setTemplateProcessor(
          [config](const String &var){
            String  ret;
            if      (var == "STA0SSID")       { ret=String(config.StaCfg[0].StaSsid); }
            else if (var == "STA0PWD")        { ret=String(config.StaCfg[0].StaPassphrase); }
            else if (var == "STA1SSID")       { ret=String(config.StaCfg[1].StaSsid); }
            else if (var == "STA1PWD")        { ret=String(config.StaCfg[1].StaPassphrase); }
            else if (var == "SERVERHOSTNAME") { ret=String(config.ServerCfg.hostname); }
            else if (var == "ADC_DOOR_UP")    { ret=String(config.ApplCfg.door_up); }
            else if (var == "ADC_DOOR_MID")   { ret=String(config.ApplCfg.door_mid); }
            else if (var == "ADC_DOOR_DWN")   { ret=String(config.ApplCfg.door_down); }     
            else { ret=var; }
            return(ret);
    });

//**********************************************************************************
//**** Write config from "config.html" POSTs
    server.on("/settings", HTTP_POST, handlerSettingsPOST );

//**** Bind Filesystem to not handle each pic/js/.. request *******
  server.serveStatic(web_path, SPIFFS, "/");

//**********************************************************************************
//**** Err 404 handler: Request via STA ==> 404, File not found- *******************
//  server.onNotFound([](AsyncWebServerRequest *request){
//  request->send(404, "text/plain", F("404, File not found."));}).setFilter(ON_STA_FILTER);
//**** Err 404 handler: Requests via AP ==> redirect "/config.html" ****************
//  server.onNotFound([](AsyncWebServerRequest *request){
//    request->redirect("/config.html");}).setFilter(ON_AP_FILTER);
  server.onNotFound([](AsyncWebServerRequest *request){
    DEBUG_PRINT("WebServer: NotFound(): ON_AP_FILTER=",ON_AP_FILTER(request)); DEBUG_PRINTLN();
     if (ON_AP_FILTER(request)) { 
        request->redirect("http://" + WiFi.softAPIP().toString() + "/config.html"); }  // ON_AP => Redirect to "http://<AP_IP>/config.html"
     else {
        request->send(404, "text/plain", F("404, File not found.")); }  // ! ON_AP ==> Return: 404, Not found
  });

//**********************************************************************************
//*** Start srver ***************
// *** DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");  
  server.begin();
//**********************************************************************************
//**** Setup the DNS server redirecting all the domains to the apIP
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.setTTL(300);
  dnsServer.start(DNS_PORT, "*",  WiFi.softAPIP() );
/***
    dnsServer.setTTL(300); // modify TTL associated  with the domain name (in seconds); default is 60 seconds
  // set which return code will be used for all other domains (e.g. sending ServerFailure instead of NonExistentDomain will reduce number of queries sent by clients)
  // default is AsyncDNSReplyCode::NonExistentDomain
  dnsServer.setErrorReplyCode(AsyncDNSReplyCode::ServerFailure);
   // start DNS server for a specific domain name
  dnsServer.start(DNS_PORT, "*",  WiFi.softAPIP() );
***/
  DEBUG_PRINT("setup(): DNS started, redirection: ", WiFi.softAPIP()); DEBUG_PRINTLN();

//**********************************************************************************
//*** Start srver ***************
  setupArduinoOTA(config.ServerCfg.hostname);
}

//***************** Loop *************************
void loop() {
  if ( WiFi.status() !=  WL_CONNECTED ) {
    WifiReConnect(config.StaCfg, config.curStaConfigs, config.ServerCfg.hostname, &config.SoftApCfg );
//    waitMs(10);
//    DEBUG_PRINTS("Loop...\n");
  }
  loopArduinoOTA();               //**** Call ArduinoOTA-Handler...
//  dnsServer.processNextRequest(); //**** DNS

}
