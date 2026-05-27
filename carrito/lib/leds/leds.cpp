#include "leds.h"

// Definición de variables globales
bool intermitentesActivo = false;
unsigned long tiempoUltimoParpadeo = 0;
bool estadoParpadeo = false;
unsigned long tiempoDirApagado = 0;
bool direccionalActiva = false;

void setupLeds() {
  pinMode(LED_IZQUIERDO, OUTPUT);
  pinMode(LED_DERECHO, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  digitalWrite(LED_IZQUIERDO, LOW);
  digitalWrite(LED_DERECHO, LOW);
}

void controlarDireccionales(int ejeX) {
  // Si las intermitentes están activas, no mostrar direccionales
  if(intermitentesActivo) {
    return;
  }
  
  if(ejeX > 50) {  // Derecha
    digitalWrite(LED_DERECHO, HIGH);
    digitalWrite(LED_IZQUIERDO, LOW);
    direccionalActiva = true;
    tiempoDirApagado = millis() + 3000;
  }
  else if(ejeX < -50) {  // Izquierda
    digitalWrite(LED_DERECHO, LOW);
    digitalWrite(LED_IZQUIERDO, HIGH);
    direccionalActiva = true;
    tiempoDirApagado = millis() + 3000;
  }
  else if(!direccionalActiva) {
    digitalWrite(LED_DERECHO, LOW);
    digitalWrite(LED_IZQUIERDO, LOW);
  }
}

void controlarIntermitentes() {
  if(intermitentesActivo) {
    if(millis() - tiempoUltimoParpadeo > 500) {
      tiempoUltimoParpadeo = millis();
      estadoParpadeo = !estadoParpadeo;
      digitalWrite(LED_IZQUIERDO, estadoParpadeo);
      digitalWrite(LED_DERECHO, estadoParpadeo);
    }
  }
}

void actualizarLeds() {
  controlarIntermitentes();
  
  // Apagar direccionales después de 3 segundos
  if(direccionalActiva && millis() > tiempoDirApagado) {
    if(!intermitentesActivo) {
      digitalWrite(LED_IZQUIERDO, LOW);
      digitalWrite(LED_DERECHO, LOW);
    }
    direccionalActiva = false;
  }
}

void tocarClaxon() {
  tone(BUZZER_PIN, 2000, 300);
  Serial.println("Claxon");
}

void apagarDireccionales() {
  digitalWrite(LED_IZQUIERDO, LOW);
  digitalWrite(LED_DERECHO, LOW);
  direccionalActiva = false;
}