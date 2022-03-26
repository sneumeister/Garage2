//*********** Kleine Hilfsroutinen ****************
//*** Hier die echten functionen ******************

#pragma once
#include "littleHelpers.h"

//**********************************************************************************
//*** Warte 'interval' Millisekunden.... delay() Ersatz ****************************
void waitMs(unsigned int interval) {
  unsigned long millisinput=millis();
  while((millis()-millisinput) < interval) { /** do nothing... ***/ }
}
//**********************************************************************************
//*** Random Integer im Bereich 'min_num' -> 'max_num' (je einschliesslich) ********
int random_number(int min_num, int max_num){
  int result = 0, low_num = 0, hi_num = 0;
  if (min_num < max_num) {
    low_num = min_num;
    hi_num = max_num + 1; // include max_num in output
  } else {
    low_num = max_num + 1; // include max_num in output
    hi_num = min_num;
  }
  srand(time(NULL));
  result = (rand() % (hi_num - low_num)) + low_num;
  return result;
}
