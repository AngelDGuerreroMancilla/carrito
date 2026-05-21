#pragma once
#include <Arduino.h>

// Pines del sensor ultrasónico
#define echo 7
#define trig 42

// Distancia mínima en cm para considerar un obstáculo
#define DIST_OBSTACULO 15
#define DIST_INT       30   // Distancia de advertencia (encienden ambas intermitentes)
#define INTERVALO_PARPADEO 300  // ms entre cada toggle del LED

// Máquina de estados para la evasión de obstáculos
enum EstadoEvasion {
  SIGUIENDO_LINEA,
  DETECTADO_INT,      // Obstáculo a 30 cm → ambas intermitentes encendidas
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

// Bandera para habilitar el modo evasor independiente
extern bool modEvasor;

// Máquina de estados: sigue la línea y evade obstáculos automáticamente
void modoAutonomo();

// Máquina de estados: evasor de obstáculos libre (sin seguir línea)
void modoEvasorLibre();

// Control de intermitentes
void encenderIntermitentes();
void apagarIntermitentes();
void parpadearIntermitenteDer();
void parpadearIntermitenteIzq();
