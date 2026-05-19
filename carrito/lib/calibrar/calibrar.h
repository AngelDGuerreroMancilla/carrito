#pragma once
#include <Arduino.h>
#include <QTRSensors.h>

#define echo 7
#define trig 42 

//bandera de modo calibracion
extern bool calibracion;

extern QTRSensors qtr;
extern const uint8_t sensorCount;
extern uint8_t pinesSensores[];

void calibrar();
