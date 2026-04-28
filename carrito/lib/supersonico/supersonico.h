#pragma once
#include <Arduino.h>

// Pines del sensor ultrasónico
#define echo 7
#define trig 42

// Distancia mínima en cm para considerar un obstáculo
#define DIST_OBSTACULO 15

// Máquina de estados para la evasión de obstáculos
enum EstadoEvasion {
  SIGUIENDO_LINEA,
  DETENIDO,
  GIRANDO_DERECHA,
  AVANZANDO,
  GIRANDO_IZQUIERDA,
  BUSCANDO_LINEA
};

// Estado actual de la evasión (accesible desde main.cpp)
extern EstadoEvasion estadoActual;

// Mide la distancia en cm con el sensor ultrasónico
int medirDistancia();

// Detiene los 2 motores (freno de emergencia por obstáculo)
void detenerMotores();

// Retorna true si cualquier sensor IR detecta la línea
bool lineaDetectada();

// Máquina de estados: sigue la línea y evade obstáculos automáticamente
void modoAutonomo();
