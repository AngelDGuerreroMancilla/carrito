

#include <Arduino.h>
#include <PubSubClient.h> 
extern bool modSegLin;
extern bool gpsIsActive;
extern float kp, kd, ki;
extern int velBas, velMax;
extern double latDestino, lngDestino;
extern bool calibracion;

// Prototipos de funciones
void recibirAlerta(char* topic, byte* payload, unsigned int length);
bool separarTextoComa(String mensaje, double &valor1, double &valor2);
