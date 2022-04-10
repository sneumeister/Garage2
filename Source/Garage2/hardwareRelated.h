//*********** Alles zur Hardware: ADC und Pins **********
//*******************************************************

#pragma once

#define DEBUGINFO 0           // Compile DEBUG clauses....

#include <Arduino.h>
#include <driver/adc.h>
#include "debug.h"            // Switch on/of Debug Info via 'Serial'
#include "littleHelpers.h"    // Hilfs-Funktion.. z.B. waitMs()
#include "config.h"           // Globale Config-Datei

//*** AD Converter settings ********************************
const adc1_channel_t    cfg_adc_input = ADC1_CHANNEL_4;     /*!< ADC1 channel 4 is GPIO32 */
const adc_atten_t       cfg_adc_attn  = ADC_ATTEN_DB_11;    /*!<The input voltage of ADC will be reduced to about 1/3.6*/
const adc_bits_width_t  cfg_adc_width = ADC_WIDTH_12Bit;    /*!< ADC capture width is 12Bit*/
const unsigned int      cfg_adc_mask = 0b1111111111111111;  /* Mask out some bits.... if necessary...  */
//*** Relais settings **************************************
const int cfg_relais_pin      = 25;      // An diesem Pin hängt das Relais...
const int cfg_relais_active   = LOW;     // Wann schaltet Relais bei LOW oder HIGH am Pin?
//*** Signal LED  ******************************************
const int cfg_signal_led      = 13;      // No BuiltIn LED on ESP32U-Board.. hier Signal-LED...
//const int cfg_signal_led      = 2;         // Onboard LED bei diversen Borads...
const int cfg_signal_active   = HIGH;    // HIGH == ON
//********************* Globalvars **********
//int door_direct;                         // Opening the door will increase or decrese ADC ?
//int door_stat_limit[3];                  // limits to judge door level
//bool  connected_one = false;
//int   door_status = 0;

//******************* Function prototypes *************************************
bool init_hardware();                                 //**** Initialisiere Herawre Pins und ADC...
int read_door_adc();                                  //**** Read ADC for door and MASK it
int door_level(int adc_val, const ApplConfig &ApCfg); //**** Convert read ADC into Level 0-3: 0==geschlossen... 3==offen
void push_the_button();                               //**** Switch Door relais => open/close
void signalLed(const char *signal);                   //*** string für Signal LED "." =>short blink , "-" =>long blink
