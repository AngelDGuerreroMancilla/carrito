#include <Arduino.h>
#include <WiFi.h>

#include <QTRSensors.h>
#include <Preferences.h>
#include <TinyGPS++.h>
#include "motores.h"
#include "gps.h"
#include "broaker.h"
#include "calibrar.h"
#include "seguidor.h"
#include "supersonico.h"

// unsigned long duracion;
int distancia;
long ultima_medicion=0;
bool modSegLin = false;
bool calibracion=0;


bool gpsIsActive= 0;



bool separarTextoComa(String mensaje, double &valor1, double &valor2) {
  int indiceComa = mensaje.indexOf(",");
  
  if (indiceComa > 0) {
    String txt1 = mensaje.substring(0, indiceComa);
    String txt2 = mensaje.substring(indiceComa + 1);

    valor1 = txt1.toDouble();
    valor2 = txt2.toDouble();
    
    return true; 
  }
  
  return false; 
}

void recibirAlerta(char* topic, byte* payload, unsigned int length){
  Serial.print("mensaje del tema : ");
  Serial.println(topic);
  String msj = "";
  
  for(int i = 0 ; i< length; i++){
    msj += (char)payload[i];
  }
  if (strcmp(topic, "mi_carrito/esp32/joystick") == 0) { 
    double x, y; 
    
    // Si la función logra separar el texto...
    if (separarTextoComa(msj, x, y)) {
      // Como el joystick necesita enteros (int), los convertimos al pasarlos
      contrlJoy((int)x, (int)y); 
    }
    
  }
  if(strcmp(topic,"mi_carrito/esp32/seguidorLineas" )== 0){
    modSegLin = msj.toInt();
    if(modSegLin == 1){
      estadoActual = SIGUIENDO_LINEA; // resetea la máquina de estados al activar
    } else {
      detenerMotores(); // frena al desactivar modo autónomo
    }
    cargarCalibracion();
  }
  if(strcmp(topic,"mi_carrito/esp32/calibracion")==0){
    calibracion= msj.toInt();
    if (calibracion== 1){
      calibrar();
    }
  }
  if(strcmp(topic, "mi_carrito/esp32/kp")== 0){
    kp= msj.toFloat();
    Serial.println("kp estableido a "); Serial.println(kp);
  }
  if(strcmp(topic, "mi_carrito/esp32/kd")== 0){
    kd= msj.toFloat();
    Serial.println("kd establecido a "); Serial.println(kd);
  }  
  if(strcmp(topic, "mi_carrito/esp32/ki")== 0){
    ki= msj.toFloat();
    Serial.println("ki establecido a "); Serial.println(ki);
  }
  if(strcmp(topic, "mi_carrito/esp32/velBas")== 0){
    velBas= msj.toInt();
    Serial.println("velBas establecido a "); Serial.println(velBas);
  }
  if(strcmp(topic, "mi_carrito/esp32/velMax")== 0){
    velMax= msj.toInt();
    Serial.println("velMax establecido a "); Serial.println(velMax);
  } 
  if(strcmp(topic, "mi_carrito/esp32/actGps")== 0){
    gpsIsActive= msj.toInt();
    Serial.print("gps "); Serial.println(msj.toInt());
  }
  if (strcmp(topic, "mi_carrito/esp32/dest") == 0) { 
    
    if (separarTextoComa(msj, latDestino, lngDestino)) {
      
      Serial.print("Nuevo destino fijado: ");
      Serial.print(latDestino, 6);
      Serial.print(", ");
      Serial.println(lngDestino, 6);
      
    }
  }
}




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
  pinMide()

 
  apagar();

  qtr.setTypeAnalog();
  qtr.setSensorPins(pinesSensores, SensorCount);
  preferencias.begin("datos_qtr", false);
  cargarCalibracion();
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
  

  if(modSegLin== 1){
    modoAutonomo();   // verifica obstáculos y controla evasión
    if(estadoActual == SIGUIENDO_LINEA) {
      contrLineas();  // sigue la línea solo si no hay obstáculo
    }
  }
  client.loop();
  long ahora=millis();

  
}
