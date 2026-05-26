#include "gps.h"
#include "broaker.h"
#include "motores.h"

TinyGPSPlus gps;
double lat  = 0.0;
double lng  = 0.0;
double latDestino = 0.0;
double lngDestino = 0.0;

// ─────────────────────────────────────────────
//  MÁQUINA DE ESTADOS DE NAVEGACIÓN
//  ESPERA → AVANZANDO → CORRIGIENDO → AVANZANDO …
//
//  Lógica clave:
//   • Primero avanza (genera vector real de rumbo)
//   • Luego corrige si hay error angular
//   • NUNCA corrige sin haber medido rumbo antes
// ─────────────────────────────────────────────
enum EstadoNav {
  NAV_ESPERA,       // Sin destino activo
  NAV_AVANZANDO,    // Moviéndose recto para medir rumbo
  NAV_CORRIGIENDO   // Girando para apuntar al destino
};

EstadoNav estadoNav          = NAV_ESPERA;
unsigned long tInicioEstado  = 0;

double latAnterior  = 0.0;
double lonAnterior  = 0.0;
double rumboActual  = 0.0;
bool   rumboValido  = false;

// ── Parámetros de navegación ──────────────────
const unsigned long MS_AVANCE      = 6000;  // ms avanzando antes de medir
const unsigned long MS_CORRECCION  = 600;  // ms máx girando por ciclo
const double DIST_LLEGADA          = 2.5;   // metros para declarar llegada
const double MIN_DESP              = 0.25;  // metros mínimos para aceptar vector
const double TOL_GIRO              = 20.0;  // grados de tolerancia de rumbo

// ─────────────────────────────────────────────}

void cambiarEstado(EstadoNav nuevo, const char* nombre) {
  estadoNav      = nuevo;
  tInicioEstado  = millis();
  Serial.print("[NAV] → "); Serial.println(nombre);
}

// ─────────────────────────────────────────────
void estadoGps() {
  static unsigned long ultimo = 0;
  if (millis() - ultimo > 2000) {
    ultimo = millis();
    Serial.print(gps.location.isValid() ? "GPS OK | Satélites: "
                                        : "GPS sin fix | Satélites visibles: ");
    Serial.println(gps.satellites.value());
    if (gps.location.isValid())
      envSig("satel", String(gps.satellites.value()));
  }
}

// ─────────────────────────────────────────────
void envPos() {
  if (!gps.location.isValid()) return;
  lat = gps.location.lat();
  lng = gps.location.lng();
  Serial.print("Lat: "); Serial.print(lat, 6);
  Serial.print(" | Lng: "); Serial.println(lng, 6);
  envSig("lat", String(lat, 6));
  envSig("lng", String(lng, 6));
}

// ─────────────────────────────────────────────
void direccionamiento() {

  // Sin destino o sin señal GPS → nada que hacer
  if (latDestino == 0.0 || !gps.location.isValid()) return;

  lat = gps.location.lat();
  lng = gps.location.lng();

  // ── ¿Llegamos? ──────────────────────────────
  double distDestino = gps.distanceBetween(lat, lng, latDestino, lngDestino);
  if (distDestino < DIST_LLEGADA) {
    apagar();
    latDestino  = 0.0;
    rumboValido = false;
    estadoNav   = NAV_ESPERA;
    Serial.println("[NAV] ¡DESTINO ALCANZADO!");
    envSig("estado", "llegada");
    return;
  }

  unsigned long tEnEstado = millis() - tInicioEstado;

  switch (estadoNav) {

    // ── ESPERA: iniciar primera calibración ───
    case NAV_ESPERA:
      latAnterior = lat;
      lonAnterior = lng;
      cambiarEstado(NAV_AVANZANDO, "AVANZANDO (calibracion inicial)");
      movDel();
      break;

    // ── AVANZANDO: generar vector de rumbo ────
    case NAV_AVANZANDO:
      movDel();

      if (tEnEstado >= MS_AVANCE) {
        double desp = gps.distanceBetween(latAnterior, lonAnterior, lat, lng);
        Serial.print("[NAV] Desplazamiento medido: "); Serial.print(desp, 2); Serial.println(" m");

        if (desp >= MIN_DESP) {
          // Vector confiable → actualizar rumbo real
          rumboActual = gps.courseTo(latAnterior, lonAnterior, lat, lng);
          rumboValido = true;
          Serial.print("[NAV] Rumbo real actualizado: "); Serial.println(rumboActual, 1);
        } else {
          // El robot casi no se movió (obstáculo, resbalón, GPS drift)
          // Mantener el último rumbo válido y reintentar avanzar
          Serial.println("[NAV] Desplazamiento insuficiente, reintentando avance...");
        }

        // Guardar posición base para el próximo ciclo
        latAnterior = lat;
        lonAnterior = lng;

        cambiarEstado(NAV_CORRIGIENDO, "CORRIGIENDO");
      }
      break;

    // ── CORRIGIENDO: girar hacia el destino ───
    case NAV_CORRIGIENDO: {

      // Sin rumbo válido todavía → volver a avanzar a ciegas
      if (!rumboValido) {
        cambiarEstado(NAV_AVANZANDO, "AVANZANDO (sin rumbo valido aun)");
        break;
      }

      double rumboDestino = gps.courseTo(lat, lng, latDestino, lngDestino);
      double error = rumboDestino - rumboActual;

      // Normalizar a [-180, 180]
      if (error >  180) error -= 360;
      if (error < -180) error += 360;

      Serial.print("[NAV] RumboDestino="); Serial.print(rumboDestino, 1);
      Serial.print(" | RumboActual=");     Serial.print(rumboActual,  1);
      Serial.print(" | Error=");           Serial.println(error, 1);

      // Tiempo de corrección agotado o rumbo aceptable → avanzar de nuevo
      if (tEnEstado >= MS_CORRECCION || abs(error) < TOL_GIRO) {
        cambiarEstado(NAV_AVANZANDO, "AVANZANDO");
        break;
      }

      // Girar hacia el destino
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
