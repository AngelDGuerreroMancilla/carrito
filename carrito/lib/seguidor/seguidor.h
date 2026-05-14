#pragma once
#include <Arduino.h>
#include "calibrar.h"

extern int lastError ;
extern float integral;

extern float kp;  
extern float kd;   
extern float ki;
extern bool modSegLin;

extern int velBas; // Velocidad normal de los motores en línea recta (0-255)
extern int velMax; 

void contrLineas();