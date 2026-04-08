#pragma once
#include <Arduino.h>

// Definimos los pines de los motores aquí
#define drvIn1 15
#define drvIn2 10
#define drvIn3 18
#define drvIn4 46

// Lista de funciones que existirán (El Menú)
void inicializarMotores(); // ¡Esta es nueva! La usaremos en el setup()
void apagar();
void movDel();
void movAtras();
void movIzq();
void movDer();
void contrlJoy(int ejeX, int ejeY);
void setMotor(int izq, int der);