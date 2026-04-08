#include <Arduino.h>
#include <WiFi.h>

#include <QTRSensors.h>
#include <Preferences.h>
#include <TinyGPS++.h>
#include "motores.h"
#include "gps.h"
#include "broaker.h"

#define echo 7
#define trig 42

#define lin1 4
#define lin2 5
#define lin3 6
#define lin4 8
#define lin5 3
#define lin6 9
#define lin7 1
#define lin8 2

QTRSensors qtr;
Preferences preferencias;

const uint8_t SensorCount = 8;
uint8_t pinesSensores[SensorCount] = {4, 5, 6, 8, 3, 9, 1, 2};

// unsigned long duracion;
int distancia;
long ultima_medicion=0;
bool modSegLin = false;
bool calibracion=0;

int lastError = 0;
float integral=0;

float kp = 0.1;  
float kd = 0.3;   
float ki = 0;
int velBas = 100; // Velocidad normal de los motores en línea recta (0-255)
int velMax = 120; // Límite de PWM

bool gpsIsActive= 0;







// void controlSensor( int distSensUlt){
//   if (distSensUlt <= 20){
    
//     analogWrite(drvIn1, 0); 
//     digitalWrite(drvIn2, LOW);
    
//     analogWrite(drvIn3, 0);
//     digitalWrite(drvIn4, LOW);
    
//     Serial.println("Freno Activado (Motores apagados)");
    
//   }else{
    
//     digitalWrite(drvIn2, LOW);
//     analogWrite(drvIn1, 200); // velocidad 
    
//     analogWrite(drvIn3, 200); // velocidad 
    
//     Serial.println("Avanzando");
//   }
// }



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



void contrLineas(){

  uint16_t valores[8]; 
  uint16_t posicion = qtr.readLineBlack(valores);

  int error = posicion - 3500;

  float errorNorm = error / 3500.0;

  // Integral controlado
  if (abs(error) < 1000) {
    integral += errorNorm;
  }

  integral = constrain(integral, -10, 10);

  float derivada = errorNorm - (lastError / 3500.0);

  float ajuste = kp * errorNorm + ki * integral + kd * derivada;

  lastError = error;

  int motorDer = velBas + (ajuste * velMax);
  int motorIzq = velBas - (ajuste * velMax);

  motorIzq = constrain(motorIzq, -velMax, velMax);
  motorDer = constrain(motorDer, -velMax, velMax);

  setMotor(motorIzq, motorDer);
}
void calibrar(){  
  Serial.println("\n*** INICIANDO CALIBRACION ***");
  Serial.println("¡MUEVE EL ROBOT DE LADO A LADO SOBRE LA LINEA!");
  
  // Leemos 400 veces (toma aprox. 10 segundos)
  for (uint16_t i = 0; i < 400; i++) {
    qtr.calibrate();
  }
  
  Serial.println("Calibracion terminada. Guardando en memoria...");

  
  for (uint8_t i = 0; i < SensorCount; i++) {
    preferencias.putUInt(("min" + String(i)).c_str(), qtr.calibrationOn.minimum[i]);
    preferencias.putUInt(("max" + String(i)).c_str(), qtr.calibrationOn.maximum[i]);
  }
  
  Serial.println("¡Datos guardados con éxito!");
}

void cargarCalibracion(){
  Serial.println("\nCargando calibracion desde memoria...");
  
  qtr.calibrate(); 

  for (uint8_t i = 0; i < SensorCount; i++) {
    // Si no hay datos, ponemos el mínimo en 1023 y el máximo en 1023 por seguridad
    qtr.calibrationOn.minimum[i] = preferencias.getUInt(("min" + String(i)).c_str(), 1023);
    qtr.calibrationOn.maximum[i] = preferencias.getUInt(("max" + String(i)).c_str(), 1023);
  }
  Serial.println("¡Calibracion cargada y lista para correr!");
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
    cargarCalibracion();
  }
  if(strcmp(topic,"mi_carrito/esp32/calibracion")==0){
    calibracion= msj.toInt();
    if (calibracion== 1){
      calibrar();
    }else{
      
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
    double latDestino, lngDestino; 
    
    if (separarTextoComa(msj, latDestino, lngDestino)) {
      
      Serial.print("Nuevo destino fijado: ");
      Serial.print(latDestino, 6);
      Serial.print(", ");
      Serial.println(lngDestino, 6);
      
    }
  }
}


// void distCm(){ 
//   digitalWrite(trig, LOW);
//   delayMicroseconds(2);
//   digitalWrite(trig, HIGH);
//   delayMicroseconds(10);
//   digitalWrite(trig, LOW);
//   duracion = pulseIn(echo, HIGH,30000);
  
//   int distCm = (duracion * 0.0343/ 2 );
//   if(duracion == 0 ){
//     distCm = 500;
//   }
//   controlSensor(distCm);
  
// }


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
    while(Serial2.available() > 0) {
      if (gps.encode(Serial2.read())) {
       
        envPos(); 
      }
    }
  }
  estadoGps();

  if(modSegLin== 1){
      contrLineas();
  }
  client.loop();
  long ahora=millis();
  // if (ahora-ultima_medicion>50){
  //   ultima_medicion = ahora;
  //   // distCm();
    
   
  // }
  
}
