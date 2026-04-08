#pragma once
#include <Arduino.h>

#define RXD2 16
#define TXD2 17

extern TinyGPSPlus gps;

void estadoGps();
void envPos();