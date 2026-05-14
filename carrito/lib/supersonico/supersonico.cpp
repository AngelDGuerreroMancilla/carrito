#include "supersonico.h"
#include "motores.h"
#include "seguidor.h"

// Estado inicial de la máquina de evasión
EstadoEvasion estadoActual = SIGUIENDO_LINEA;
static unsigned long tiempoEstado = 0;

// Mide la distancia en cm con el sensor ultrasónico
int medirDistancia() {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  unsigned long duracion = pulseIn(echo, HIGH, 30000);
  if (duracion == 0) return 500;
  return (int)(duracion * 0.0343 / 2);
}

// Detiene los 2 motores (freno de emergencia por obstáculo)
void detenerMotores() {
  apagar();
}

// Retorna true si cualquier sensor IR detecta la línea
// Usa los pines de sensores definidos en seguidor.h/calibrar.h
bool lineaDetectada() {
  // Lee la posición del QTR: si es distinta de 0 y 7000, hay línea
  // Alternativa simple: revisar si al menos un sensor detecta negro
  uint16_t valores[8];
  extern QTRSensors qtr;
  uint16_t posicion = qtr.readLineBlack(valores);
  // Si la posición está entre los extremos, hay línea detectada
  return (posicion > 500 && posicion < 6500);
}

// Máquina de estados: evade obstáculos automáticamente
void modoAutonomo() {
  unsigned long ahora = millis();

  switch (estadoActual) {
    case SIGUIENDO_LINEA: {
      int dist = medirDistancia();
      if (dist <= DIST_OBSTACULO) {
        detenerMotores();
        estadoActual = DETENIDO;
        tiempoEstado = ahora;
        Serial.println("Obstaculo detectado! Deteniendo...");
      }
      // El seguimiento de línea lo gestiona contrLineas() en main.cpp
      break;
    }

    case DETENIDO:
      detenerMotores();
      if (ahora - tiempoEstado >= 200) {
        estadoActual = GIRANDO_DERECHA;
        tiempoEstado = ahora;
        Serial.println("Girando a la derecha...");
      }
      break;

    case GIRANDO_DERECHA:
      movDer();
      if (ahora - tiempoEstado >= 400) {
        estadoActual = AVANZANDO;
        tiempoEstado = ahora;
        Serial.println("Avanzando para pasar obstaculo...");
      }
      break;

    case AVANZANDO:
      movDel();
      if (ahora - tiempoEstado >= 500) {
        estadoActual = GIRANDO_IZQUIERDA;
        tiempoEstado = ahora;
        Serial.println("Girando a la izquierda...");
      }
      break;

    case GIRANDO_IZQUIERDA:
      movIzq();
      if (ahora - tiempoEstado >= 400) {
        estadoActual = BUSCANDO_LINEA;
        tiempoEstado = ahora;
        Serial.println("Buscando linea...");
      }
      break;

    case BUSCANDO_LINEA:
      movDelIzq();
      if (lineaDetectada()) {
        estadoActual = SIGUIENDO_LINEA;
        Serial.println("Linea encontrada! Reanudando seguimiento.");
      } else if (ahora - tiempoEstado >= 2000) {
        // Timeout: girar más a la izquierda para buscar la línea
        estadoActual = GIRANDO_IZQUIERDA;
        tiempoEstado = ahora;
        Serial.println("Timeout buscando linea, girando mas...");
      }
      break;
  }
}
