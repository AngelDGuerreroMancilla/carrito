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
      Serial.print("GPS: ¡Conectado! Satélites en uso: ");
      Serial.println(gps.satellites.value());
      
      // Si quieres mandar este aviso a tu web, descomenta estas líneas:
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

  if (latDestino == 0.0 || !gps.location.isValid()) {
    return; 
  }

double dist=gps.distanceBetween(lat,lng,latDestino,lngDestino);
double gradDes= gps.courseTo(lat,lng,latDestino,lngDestino);

double gradAct= gps.course.deg();


if (dist<=2 ){
  apagar();
  Serial.println("llegada a objetivo, esta a menos de 2M");
  latDestino=0.0;
  return;
}



  double errorGiro= gradDes-gradAct;


  if (errorGiro > 180){
    errorGiro -= 360;
  }
  if(errorGiro < -180){
    errorGiro += 360;
  }

  if(abs(errorGiro)<15){
    movDel();
    Serial.println("gps mov adelante");

  }else if(errorGiro>0){
    movDer();
    Serial.println("gps mov derecha");

  }else {
    movIzq();
    Serial.println("gps mov izquierda");
  }

}
