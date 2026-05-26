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

// unsigned long duracion;
int distancia;
long ultima_medicion=0;





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


 
  apagar();

  calibrar();
  setup_wifi();
  
  client.setServer(mqttServer, 1883);
  client.setCallback(recibirAlerta);
}


void loop(){
  // 1. Mantener conexión MQTT
  if(!client.connected()){
    reconnect();
  }
  client.loop(); // Es buena práctica poner el loop de MQTT al principio o justo después del reconnect

  // 2. Modo GPS
  if(gpsIsActive) {
    estadoGps();
    
    while(Serial2.available() > 0) {
      gps.encode(Serial2.read());
    }
    
    if (gps.location.isUpdated()) {
      envPos(); 
      direccionamiento();
    }
  }
  
  // 3. Modo Seguidor de Línea
  if(modSegLin == 1){
      contrLineas();
  }

  // 4. Lectura de Ultrasonido (No bloqueante)
  long ahora = millis();
  if (ahora - ultima_medicion > 100) { // Leer cada 100ms
    ultima_medicion = ahora;
    
    // Aquí iría tu lógica de lectura del trigger y echo...
    // distancia = calcularDistancia();
    
    // if (distancia < 15) { apagar(); } // Paro de emergencia
  }
}