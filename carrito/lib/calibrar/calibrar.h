#pragma once
#include <Arduino.h>
#include <QTRSensors.h>
#include <Preferences.h>

#define echo 7
#define trig 42
extern QTRSensors qtr;
extern Preferences preferencias;
extern const uint8_t SensorCount;
extern uint8_t pinesSensores[];

void calibrar();
void cargarCalibracion();
