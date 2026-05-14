#ifndef gps_h
#define gps_h
#pragma once
#include <Arduino.h>
#include <TinyGPS++.h>
#include <PubSubClient.h>

#define RXD2 16
#define TXD2 17

extern TinyGPSPlus gps;
extern double latDestino;
extern double lngDestino;
extern bool gpsIsActive;



void estadoGps();
void envPos();
void direccionamiento();
#endif