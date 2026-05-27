#ifndef LEDS_H
#define LEDS_H

#include <Arduino.h>

// Definición de pines
#define LED_IZQUIERDO 36
#define LED_DERECHO 37
#define BUZZER_PIN 35

// Variables globales
extern bool intermitentesActivo;

// Prototipos
void setupLeds();
void controlarDireccionales(int ejeX);
void controlarIntermitentes();
void actualizarLeds();
void tocarClaxon();
void apagarDireccionales();

#endif