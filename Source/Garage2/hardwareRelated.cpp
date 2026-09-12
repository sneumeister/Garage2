//******** Funktionen zur Hardware: ADC und Pins **********
//*********************************************************

#include "hardwareRelated.h"

//***************************************************************

static adc_oneshot_unit_handle_t adc_handle = nullptr;

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
  
  //*** Inititialisiere ADC (oneshot)...
  DEBUG_PRINT("Initialize ADC (unit ", cfg_adc_unit);
  DEBUG_PRINT(", ch ", cfg_adc_channel); DEBUG_PRINTS("): ");
  adc_oneshot_unit_init_cfg_t init_config = {
    .unit_id = cfg_adc_unit,
    .clk_src = ADC_RTC_CLK_SRC_DEFAULT,
    .ulp_mode = ADC_ULP_MODE_DISABLE,
  };
  if (ESP_OK != adc_oneshot_new_unit(&init_config, &adc_handle)) {
    DEBUG_PRINTS("Error creating oneshot unit.\n");
    ret=false;
  } else {
    adc_oneshot_chan_cfg_t chan_config = {
      .atten = cfg_adc_atten,
      .bitwidth = cfg_adc_bitwidth,
    };
    if (ESP_OK != adc_oneshot_config_channel(adc_handle, cfg_adc_channel, &chan_config)) {
      DEBUG_PRINTS("Error configuring channel.\n");
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
  int raw = 0;
  if (adc_handle == nullptr || ESP_OK != adc_oneshot_read(adc_handle, cfg_adc_channel, &raw)) {
    DEBUG_PRINTS("read_door_adc: oneshot read failed.\n");
    return 0;
  }
  return (raw & cfg_adc_mask);
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
  vTaskDelay( 500 / portTICK_PERIOD_MS);
  digitalWrite(cfg_relais_pin, !(cfg_relais_active));
}
//***************************************************************
//*** Signal LED **** string "...---..." == "SOS"
void signalLed(const char *signal) {
  const int Pshort= 20 / portTICK_PERIOD_MS;     // (time in ms)
  const int Plong=  70 / portTICK_PERIOD_MS;     // (time in ms)
  const int Ppause= 50 / portTICK_PERIOD_MS;     // (time in ms)

  for(int i=0; i<strlen(signal); i++) {
    switch(signal[i]) {
      case '.':
        digitalWrite(cfg_signal_led, cfg_signal_active);
        vTaskDelay( Pshort );
        break;
      case '-':
        digitalWrite(cfg_signal_led, cfg_signal_active);
        vTaskDelay( Plong );
        break;
      default:  //** additional Pause if unknown symbol....
        vTaskDelay( Ppause );
        break;
    }
    digitalWrite(cfg_signal_led, !(cfg_signal_active));
    vTaskDelay( Ppause );
  }
}

void signalLedEnqueue(const char *msg) {
  if (signalLedQueue == NULL || msg == NULL) return;
  char buf[MAX_SIGNAL_MSG_LEN];
  strlcpy(buf, msg, sizeof(buf));
  xQueueSend(signalLedQueue, buf, 150 / portTICK_PERIOD_MS);
}

void signalLedTask(void * parameter){
  const int OnShort   = 30  / portTICK_PERIOD_MS;     // (time in ms) Short-On =  .
  const int OnLong    = 70  / portTICK_PERIOD_MS;     // (time in ms) Long-On  =  *
  const int OffShort  = 30  / portTICK_PERIOD_MS;     // (time in ms) Short-Off=  -
  const int OffLong   = 70  / portTICK_PERIOD_MS;     // (time in ms) Long-Off =  =

  if (signalLedQueue == NULL) {
    vTaskDelete(NULL);
    return;
  }

  char  msg[MAX_SIGNAL_MSG_LEN];
  for(;;) {
    xQueueReceive(signalLedQueue, msg, portMAX_DELAY);
    for(int i=0; (i<strlen(msg)) && (i< MAX_SIGNAL_MSG_LEN ); i++) {
      switch(msg[i]) {
      case '.':           //** Short-On
        digitalWrite(cfg_signal_led, cfg_signal_active);
        vTaskDelay( OnShort );
        break;
      case '*':           //** Long-On
        digitalWrite(cfg_signal_led, cfg_signal_active);
        vTaskDelay( OnLong );
        break;
      case '-':           //** Short-Off
        digitalWrite(cfg_signal_led, !(cfg_signal_active));
        vTaskDelay( OffShort );
        break;
      case '=':           //** Long-Off
        digitalWrite(cfg_signal_led, !(cfg_signal_active));
        vTaskDelay( OffLong );
        break;       
      default:  //** Undefined char... do nothing
        break;
      }
    }
    digitalWrite(cfg_signal_led, !(cfg_signal_active));
    vTaskDelay( OffLong );
   }
};
