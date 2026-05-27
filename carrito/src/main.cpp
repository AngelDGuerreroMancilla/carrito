#include <Arduino.h>
#include <WiFi.h>

#include <QTRSensors.h>
#include <TinyGPS++.h>
#include "motores.h"
#include "gps.h"
#include "broaker.h"
#include "calibrar.h"
#include "seguidor.h"
#include "pubSub/pubSub.h"
#include "supersonico.h"
#include "leds.h"  
// modoAutonomo(), estadoActual, DETECTADO_INT, LEDs

// unsigned long duracion;
int distancia;
long ultima_medicion=0;
extern bool intermitentesActivo;





void setup(){
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  delay(1000);  


  analogReadResolution(10);
  pinMode(echo, INPUT);
  pinMode(trig, OUTPUT);
  pinMode(drvIn1, OUTPUT);
  pinMode(drvIn2, OUTPUT);
  pinMode(drvIn3, OUTPUT);
  pinMode(drvIn4, OUTPUT);

  pinMode(LED_IZQ, OUTPUT);
  pinMode(LED_DER, OUTPUT);
  digitalWrite(LED_IZQ, LOW);
  digitalWrite(LED_DER, LOW);


  // LEDs intermitentes
  pinMode(ledDer, OUTPUT);  digitalWrite(ledDer, LOW);
  pinMode(ledIzq, OUTPUT);  digitalWrite(ledIzq, LOW);
  apagar();

  calibrar();
  setup_wifi();
  
  client.setServer(mqttServer, 1883);
  client.setCallback(recibirAlerta);
}


void loop(){
  // 1. Mantener la conexión MQTT activa
  if(!client.connected()){
    reconnect();
  }
<<<<<<< HEAD
  client.loop();

  // 2. LEER GPS CONSTANTEMENTE (Siempre debe vaciar el buffer serial)
  while(Serial2.available() > 0) {
    gps.encode(Serial2.read());
=======
  actualizarLeds();
  
  if(gpsIsActive) {
    estadoGps();
    while(Serial2.available() > 0) {
      if (gps.encode(Serial2.read())) {
        envPos(); 
        direccionamiento();
      }
    }
  }

  // 3. MÁQUINA DE ESTADOS / CONTROL DE MODOS
  if (gpsIsActive == 1) {
    estadoGps(); // Reporta satélites cada 2 segundos
    envPos();    // Envía posición MQTT solo si se actualizó el GPS
    
    // Solo permitimos que el GPS controle motores si no están activos los otros modos
    if (modSegLin == 0 && modEvasor == 0) {
      direccionamiento(); 
    }
  } 
  else if (modSegLin == 1) {
    modoAutonomo();   // gestiona distancias e intermitentes
    if(estadoActual == SIGUIENDO_LINEA || estadoActual == DETECTADO_INT){
      contrLineas();
    }
  } 
  else if (modEvasor == 1) {
    modoEvasorLibre(); // Modo independiente para evitar obstáculos
  }
  else {
    // Si ningún modo está activo, asegurar que el vehículo esté detenido
    apagar(); 
  }
}
