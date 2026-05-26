// lib/gps/gps.cpp
#include "gps.h"
#include "broaker.h"
#include "motores.h"

TinyGPSPlus gps;
double lat       = 0.0;
double lng       = 0.0;
double latDestino = 0.0;
double lngDestino = 0.0;
bool   hayDestino = false;

enum EstadoNav {
  NAV_ESPERA,
  NAV_AVANZANDO,
  NAV_CORRIGIENDO
};

EstadoNav     estadoNav       = NAV_ESPERA;
unsigned long tInicioEstado   = 0;

double latAnterior = 0.0;
double lonAnterior = 0.0;
double rumboActual = 0.0;
bool   rumboValido = false;

// ── Parámetros ────────────────────────────────
const unsigned long MS_AVANCE     = 3000;  // tiempo avanzando para medir rumbo
const unsigned long MS_CORRECCION = 1500;  // tiempo máximo girando por ciclo  ← era 600
const double DIST_LLEGADA         = 2.5;
const double MIN_DESP             = 0.10;  // ← era 0.25, más tolerante
const double TOL_GIRO             = 15.0;  // grados aceptables de error

void cambiarEstado(EstadoNav nuevo, const char* nombre) {
  estadoNav     = nuevo;
  tInicioEstado = millis();
  Serial.print("[NAV] → "); Serial.println(nombre);
}

void estadoGps() {
  static unsigned long ultimo = 0;
  if (millis() - ultimo > 2000) {
    ultimo = millis();
    Serial.print(gps.location.isValid() ? "GPS OK | Satélites: "
                                        : "GPS sin fix | Satélites: ");
    Serial.println(gps.satellites.value());
    if (gps.location.isValid())
      envSig("satel", String(gps.satellites.value()));
  }
}

void envPos() {
  if (!gps.location.isValid()) return;
  lat = gps.location.lat();
  lng = gps.location.lng();
  Serial.print("Lat: "); Serial.print(lat, 6);
  Serial.print(" | Lng: "); Serial.println(lng, 6);
  envSig("lat", String(lat, 6));
  envSig("lng", String(lng, 6));
}

void direccionamiento() {

  if (!hayDestino || !gps.location.isValid()) return;  // ← usa hayDestino

  lat = gps.location.lat();
  lng = gps.location.lng();

  // ── ¿Llegamos? ──────────────────────────────
  double distDestino = gps.distanceBetween(lat, lng, latDestino, lngDestino);
  Serial.print("[NAV] Distancia al destino: "); Serial.print(distDestino, 1); Serial.println(" m");

  if (distDestino < DIST_LLEGADA) {
    apagar();
    hayDestino  = false;   // ← limpia con bandera
    latDestino  = 0.0;
    lngDestino  = 0.0;
    rumboValido = false;
    estadoNav   = NAV_ESPERA;
    Serial.println("[NAV] ¡DESTINO ALCANZADO!");
    envSig("estado", "llegada");
    return;
  }

  unsigned long tEnEstado = millis() - tInicioEstado;

  switch (estadoNav) {

    case NAV_ESPERA:
      latAnterior = lat;
      lonAnterior = lng;
      cambiarEstado(NAV_AVANZANDO, "AVANZANDO (calibracion inicial)");
      movDel();
      break;

    case NAV_AVANZANDO:
      movDel();

      if (tEnEstado >= MS_AVANCE) {
        double desp = gps.distanceBetween(latAnterior, lonAnterior, lat, lng);
        Serial.print("[NAV] Desplazamiento: "); Serial.print(desp, 2); Serial.println(" m");

        if (desp >= MIN_DESP) {
          rumboActual = gps.courseTo(latAnterior, lonAnterior, lat, lng);
          rumboValido = true;
          Serial.print("[NAV] Rumbo actualizado: "); Serial.println(rumboActual, 1);

          // ── NUEVA LÓGICA: revisar si ya apunta bien ANTES de corregir ──
          double rumboDestino = gps.courseTo(lat, lng, latDestino, lngDestino);
          double errorInicial = rumboDestino - rumboActual;
          if (errorInicial >  180) errorInicial -= 360;
          if (errorInicial < -180) errorInicial += 360;

          if (abs(errorInicial) < TOL_GIRO) {
            // Ya apunta bien → seguir avanzando sin corregir
            Serial.println("[NAV] Rumbo correcto, siguiendo adelante");
            latAnterior = lat;
            lonAnterior = lng;
            cambiarEstado(NAV_AVANZANDO, "AVANZANDO (rumbo OK)");
            break;
          }
        } else {
          Serial.println("[NAV] Desplazamiento insuficiente, reintentando...");
        }

        latAnterior = lat;
        lonAnterior = lng;
        apagar();                              // ← detener antes de girar
        delay(200);
        cambiarEstado(NAV_CORRIGIENDO, "CORRIGIENDO");
      }
      break;

    case NAV_CORRIGIENDO: {

      if (!rumboValido) {
        cambiarEstado(NAV_AVANZANDO, "AVANZANDO (sin rumbo valido)");
        break;
      }

      double rumboDestino = gps.courseTo(lat, lng, latDestino, lngDestino);
      double error = rumboDestino - rumboActual;
      if (error >  180) error -= 360;
      if (error < -180) error += 360;

      Serial.print("[NAV] RumboDestino="); Serial.print(rumboDestino, 1);
      Serial.print(" | RumboActual=");     Serial.print(rumboActual, 1);
      Serial.print(" | Error=");           Serial.println(error, 1);

      // Tiempo agotado o rumbo aceptable → avanzar
      if (tEnEstado >= MS_CORRECCION || abs(error) < TOL_GIRO) {
        apagar();                              // ← detener antes de avanzar
        delay(200);
        latAnterior = lat;
        lonAnterior = lng;
        cambiarEstado(NAV_AVANZANDO, "AVANZANDO");
        break;
      }

      if (error > 0) {
        movDer();
        Serial.println("[NAV] Girando DERECHA →");
      } else {
        movIzq();
        Serial.println("[NAV] Girando IZQUIERDA ←");
      }
      break;
    }
  }
}