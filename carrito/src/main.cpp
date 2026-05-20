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
#include "supersonico.h"   // modoAutonomo(), estadoActual, DETECTADO_INT, LEDs

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
  if(!client.connected()){
    reconnect();
  }
  if(gpsIsActive) {
    estadoGps();
    while(Serial2.available() > 0) {
      if (gps.encode(Serial2.read())) {
        envPos(); 
        direccionamiento();
      }
    }
  }


  if(modSegLin == 1){
    modoAutonomo();   // gestiona distancias e intermitentes
    // contrLineas() solo corre cuando no hay maniobra de evasión activa
    if(estadoActual == SIGUIENDO_LINEA || estadoActual == DETECTADO_INT){
      contrLineas();
    }
  }

  client.loop();
  long ahora = millis();

  
}
