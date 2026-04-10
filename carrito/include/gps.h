#pragma once
#include <Arduino.h>

#define RXD2 16
#define TXD2 17

extern TinyGPSPlus gps;
extern double latDestino;
extern double lngDestino;


void estadoGps();
void envPos();
void direccionamiento();