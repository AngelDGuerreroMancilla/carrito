#pragma once
#include <Arduino.h>

// Definimos los pines de los motores aquí
#define drvIn1 15
#define drvIn2 10
#define drvIn3 18
#define drvIn4 46
#define sensVelIzq 11
#define sensVelDer 12
#define ledIzq 4
#define ledDer 2


// 
void apagar();
void movDel();
void movAtras();
void movIzq();
void movDer();
void movDelDer();
void movDelIzq();
void contrlJoy(int ejeX, int ejeY);
void setMotor(int izq, int der);
void seguirUbicacion(double cursoGrados, double destGrados);