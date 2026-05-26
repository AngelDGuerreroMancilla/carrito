#include "gps.h"
#include "broaker.h"
#include "motores.h"

TinyGPSPlus gps;

double lat ;
double lng;
double latDestino = 0.0;
double lngDestino = 0.0;
bool gpsIsActive= 0;

void estadoGps() {
 
  static long ultimo_estado_gps = 0;
  long ahora = millis();

  if (ahora - ultimo_estado_gps > 2000) {
    ultimo_estado_gps = ahora;
    if (!gps.location.isValid()) {
      Serial.print("Satélites a la vista: ");
      Serial.println(gps.satellites.value());
    } else {
      Serial.print("GPS Satélites en uso: ");
      Serial.println(gps.satellites.value());
      
     
      String msj = String(gps.satellites.value());
      envSig("satel", msj);
    }
  }
}

void envPos(){
  if(gps.location.isValid() && gps.location.isUpdated()){

    lat = gps.location.lat();
    lng = gps.location.lng();
        
    Serial.print("Latitud: "); Serial.println(lat, 6);
    Serial.print("Longitud: "); Serial.println(lng, 6);
        
    String latEnv = String(lat, 6);
    String lngEnv = String(lng, 6);
      
    envSig("lat", latEnv);
    envSig("lng", lngEnv);
  }
}

void direccionamiento(){

  // Si no hay destino o el GPS no tiene señal, salimos de la función
  if (latDestino == 0.0 || !gps.location.isValid()) {
    return; 
  }

  // 1. Calculamos la distancia
  double dist = gps.distanceBetween(lat, lng, latDestino, lngDestino);

  // 2. Verificamos si ya llegamos (¡Prioridad máxima!)
  if (dist < 2.0) {
    apagar();
    Serial.println("Llegada a objetivo, está a menos de 2M");
    latDestino = 0.0; // Reseteamos el destino
    return;           // Salimos para no seguir calculando giros
  }

  // 3. Si no hemos llegado, calculamos hacia dónde mirar
  double gradDes = gps.courseTo(lat, lng, latDestino, lngDestino);
  double gradAct = gps.course.deg(); 

  double errorGiro = gradDes - gradAct;

  // 4. Normalizamos el error (para que siempre esté entre -180 y 180 grados)
  if (errorGiro > 180) {
    errorGiro -= 360;
  } else if (errorGiro < -180) {
    errorGiro += 360;
  }

  // 5. Tomamos la decisión de movimiento
  if (abs(errorGiro) < 15) {
    movDel();
    Serial.println("GPS: En rumbo correcto, mov adelante");
  } else if (errorGiro > 0) {
    movDer();
    Serial.println("GPS: Corrigiendo a la derecha");
  } else {
    movIzq();
    Serial.println("GPS: Corrigiendo a la izquierda");
  } 
}
