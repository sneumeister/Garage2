//*********** Kleine Hilfsroutinen ****************

#pragma once
#include <Arduino.h>

//*** Warte 'interval' Millisekunden, delay() Ersatz ****************************
void waitMs(unsigned int interval);

//*** Random Integer im Bereich 'min_num' -> 'max_num' (je einschliesslich) ****
int random_number(int min_num, int max_num);
