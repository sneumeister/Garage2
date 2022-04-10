//******** Funktionen zur Hardware: ADC und Pins **********
//*********************************************************

#pragma once
#include "hardwareRelated.h"




//***************************************************************
//**** Initialisiere Hardwae Pins und ADC...
bool init_hardware() {
  bool ret=false;
  //*** Inititialisiere Relais-Pin...
  DEBUG_PRINT("Setting Relais-Pin ", cfg_relais_pin);
  DEBUG_PRINT(" to OUTPUT and ", cfg_relais_active==HIGH?"LOW":"HIGH");
  DEBUG_PRINTS(": ");
  pinMode(cfg_relais_pin, OUTPUT);
  digitalWrite(cfg_relais_pin, !(cfg_relais_active));
  DEBUG_PRINTS(" OK."); DEBUG_PRINTLN();
  
  //*** Inititialisiere LED-Pin...
  pinMode(cfg_signal_led, OUTPUT);
  digitalWrite(cfg_signal_led, !(cfg_signal_active));
  
  //*** Inititialisiere ADC...
  DEBUG_PRINT("Initialize ADC (", cfg_adc_input); DEBUG_PRINTS("): ");
  if (ESP_OK != adc1_config_width(cfg_adc_width)) {
    DEBUG_PRINTS("Error setting bit width.\n");
    ret=false;
  } else {
    if (ESP_OK != adc1_config_channel_atten( cfg_adc_input , cfg_adc_attn )) {
      DEBUG_PRINTS("Error setting capture attenuation.\n");
      ret=false;
    } else {
      ret=true;
      DEBUG_PRINTS("OK.\n");
    }
  }
  return(ret);
 }
//***************************************************************
//**** Read ADC for door and MASK it
int read_door_adc() {
  return ( adc1_get_raw( cfg_adc_input ) &  cfg_adc_mask);
}
//***************************************************************
//**** Convert read ADC into Level 0-3
int door_level(int adc_val, const ApplConfig &ApCfg) {
/**** Convert ADC-Value into Door-Status: ****************************
 *      0 = fully open
 *      1 = 2/3 open (1/3 closed)
 *      2 = 1/3 open (2/3 cloed)
 *      3 = fully closed
 *
 *      door_open_big = TRUE  ==> hoher ADC Wert: Tor OBEN , niedriger ADC Wert: Tor UNTEN
 *                      FALSE ==> hoher ADC Wert: Tor UNTEN, niedriger ADC Wert: Tor OBEN
 ********/
  bool door_open_big = (ApCfg.door_up > ApCfg.door_down );// TRUE  ==> hoher ADC Wert: Tor OBEN , niedriger ADC Wert: Tor UNTEN
  int door_level=0;                                       // für Door_level Pictogram:  0=ganz offe.... 3=ganz zu
  
 // signalLed("...");      // ..wird schon bei Web-Server-Event aufgerufen      //*** 3x short für Tor-Abfrage....
  DEBUG_PRINT("door_level: adc_val=", adc_val); DEBUG_PRINTLN();
  
  if ( door_open_big ) {     // Door_open = high_value (e.g 400....3500)
    if      ( adc_val < ApCfg.door_down ) { door_level = 3;}  // 3 = Fully closed
    else if ( adc_val < ApCfg.door_mid  ) { door_level = 2;}  // 2 = 1/3 open (2/3 closed)
    else if ( adc_val < ApCfg.door_up   ) { door_level = 1;}  // 2 = 2/3 open (1/3 closed)
    else    { door_level = 0;}                                // 1 = Fully opened ( adc_val >= ApCfg.door_up   )
  }
  else {                     // Door_open = low_value (e.g 3500....400)
    if      ( adc_val > ApCfg.door_down ) { door_level = 3;}  // 3 = Fully closed
    else if ( adc_val > ApCfg.door_mid  ) { door_level = 2;}  // 2 = 1/3 open (2/3 closed)
    else if ( adc_val > ApCfg.door_up   ) { door_level = 1;}  // 2 = 2/3 open (1/3 closed)
    else    { door_level = 0;}                                // 1 = Fully opened ( adc_val >= ApCfg.door_up   )
  }

  DEBUG_PRINT("door_level: door_level=", door_level); DEBUG_PRINTLN();
  return(door_level);
}
//***************************************************************
//**** Switch Door open/close button
void push_the_button() {
  digitalWrite(cfg_relais_pin, cfg_relais_active);
  DEBUG_PRINTS("Switch relais!"); DEBUG_PRINTLN();
  waitMs(500);
  digitalWrite(cfg_relais_pin, !(cfg_relais_active));
}
//***************************************************************
//*** Signal LED **** string "...---..." == "SOS"
void signalLed(const char *signal) {
  const int Pshort=20;     // (time in ms)
  const int Plong=50;      // (time in ms)
  const int Ppause=30;     // (time in ms)

  for(int i=0; i<strlen(signal); i++) {
    switch(signal[i]) {
      case '.':
        digitalWrite(cfg_signal_led, cfg_signal_active);
        waitMs(Pshort);
        break;
      case '-':
        digitalWrite(cfg_signal_led, cfg_signal_active);
        waitMs(Plong);
        break;
      default:  //** additional Pause if unknown symbol....
        waitMs(Ppause);
        break;
    }
    digitalWrite(cfg_signal_led, !(cfg_signal_active));
    waitMs(Ppause);
  }
}
