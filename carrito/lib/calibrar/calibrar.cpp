#include <QTRSensors.h>
#include "calibrar.h"

QTRSensors qtr;
Preferences preferencias;
bool calibracion=0;


const uint8_t SensorCount = 8;
uint8_t pinesSensores[SensorCount] = {4, 5, 6, 8, 3, 9, 1, 2};


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


