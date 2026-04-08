#include <TinyGPS++.h>
#include "gps.h"
#include "broaker.h"


TinyGPSPlus gps;
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

    double lat = gps.location.lat();
    double lng = gps.location.lng();
        
    Serial.print("Latitud: "); Serial.println(lat, 6);
    Serial.print("Longitud: "); Serial.println(lng, 6);
        
    String latEnv = String(lat, 6);
    String lngEnv = String(lng, 6);
      
    envSig("lat", latEnv);
    envSig("lng", lngEnv);
  }
}
