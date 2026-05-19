#include <QTRSensors.h>
#include <Arduino.h>
QTRSensors qtr;
const uint8_t sensorCount = 8;
uint8_t pinesSensores[sensorCount] = {4, 5, 6, 8, 3, 9, 1, 2};


void calibrar(){  
//se configura el sensor a analogico y se aginan los pines a el sensor.
  qtr.setTypeAnalog();
  qtr.setSensorPins(pinesSensores, sensorCount);

  Serial.println("calibrando");
  
  // Leemos 400 veces, aprox. 10 segundos
  for (uint16_t i = 0; i < 400; i++) {
    qtr.calibrate();
  }
  
  Serial.println("Calibracion terminada");
  
}