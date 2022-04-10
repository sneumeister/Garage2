//*********** DEBUG Macros *****************

//#define DEBUGINFO 1             // Switch debug output on and off by 1 or 0
#define DEBUGWIFI 1             // WiFi Dug-Info on Serial...

#if DEBUGINFO
#define DEBUG_PRINTS(s)   { Serial.print(F(s)); }
#define DEBUG_PRINT(s,v)  { Serial.print(F(s)); Serial.print(v); }
#define DEBUG_PRINTX(s,v) { Serial.print(F(s)); Serial.print(F("0x")); Serial.print(v, HEX); }
#define DEBUG_PRINTLN()   { Serial.print(F("\n")); }
#else
#define DEBUG_PRINTS(s)
#define DEBUG_PRINT(s,v)
#define DEBUG_PRINTX(s,v)
#define DEBUG_PRINTLN()
#endif

//*******************
