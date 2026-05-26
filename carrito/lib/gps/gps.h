#ifndef gps_h
#define gps_h
#pragma once
#include <Arduino.h>
#include <TinyGPS++.h>
#include <PubSubClient.h>

#define RXD2 16
#define TXD2 17


extern TinyGPSPlus gps;
extern double lat;
extern double lng;
extern double latDestino;
extern double lngDestino;



void estadoGps();
void envPos();
void direccionamiento();
#endif