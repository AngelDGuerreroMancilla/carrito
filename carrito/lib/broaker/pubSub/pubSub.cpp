#ifndef pubSub_h
#define pubSub_h

#include "motores.h"
#include "gps.h"
#include "calibrar.h"
#include "broaker.h" 
#include "seguidor.h"
#include "supersonico.h"

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

void recibirAlerta(char* topic, byte* payload, unsigned int length) {
    Serial.print("mensaje del tema : ");
    Serial.println(topic);
    
    String msj = "";
    for (int i = 0; i < length; i++) {
        msj += (char)payload[i];
    }

    if (strcmp(topic, "mi_carrito/esp32/joystick") == 0) {
        double x, y;
        if (separarTextoComa(msj, x, y)) {
            contrlJoy((int)x, (int)y);
        }
    }
    else if (strcmp(topic, "mi_carrito/esp32/seguidorLineas") == 0) {
        modSegLin = msj.toInt();
        if (modSegLin) modEvasor = false;
    }
    else if (strcmp(topic, "mi_carrito/esp32/evasion") == 0) {
        modEvasor = msj.toInt();
        if (modEvasor) modSegLin = false;
    }
    else if (strcmp(topic, "mi_carrito/esp32/calibracion") == 0) {
        int calibracion = msj.toInt();
        if (calibracion == 1) {
            calibrar();
        }
    }
    else if (strcmp(topic, "mi_carrito/esp32/kp") == 0) {
        kp = msj.toFloat();
        Serial.print("kp establecido a "); Serial.println(kp);
    }
    else if (strcmp(topic, "mi_carrito/esp32/kd") == 0) {
        kd = msj.toFloat();
        Serial.print("kd establecido a "); Serial.println(kd);
    }
    else if (strcmp(topic, "mi_carrito/esp32/ki") == 0) {
        ki = msj.toFloat();
        Serial.print("ki establecido a "); Serial.println(ki);
    }
    else if (strcmp(topic, "mi_carrito/esp32/velBas") == 0) {
        velBas = msj.toInt();
        Serial.print("velBas establecido a "); Serial.println(velBas);
    }
    else if (strcmp(topic, "mi_carrito/esp32/velMax") == 0) {
        velMax = msj.toInt();
        Serial.print("velMax establecido a "); Serial.println(velMax);
    }
    else if (strcmp(topic, "mi_carrito/esp32/actGps") == 0) {
        gpsIsActive = msj.toInt();
        Serial.print("gps "); Serial.println(msj.toInt());
    }
    else if (strcmp(topic, "mi_carrito/esp32/dest") == 0) {
        if (separarTextoComa(msj, latDestino, lngDestino)) {
            Serial.print("Nuevo destino fijado: ");
            Serial.print(latDestino, 6);
            Serial.print(", ");
            Serial.println(lngDestino, 6);
        }
    }
}
#endif