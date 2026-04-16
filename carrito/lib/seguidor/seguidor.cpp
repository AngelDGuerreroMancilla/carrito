#include "seguidor.h"
#include "motores.h"

int lastError = 0;
float integral=0;

float kp = 0.1;  
float kd = 0.3;   
float ki = 0;
int velBas = 100; // Velocidad normal de los motores en línea recta (0-255)
int velMax = 120; // Límite de PWM

void contrLineas(){

  uint16_t valores[8]; 
  uint16_t posicion = qtr.readLineBlack(valores);

  int error = posicion - 3500;

  float errorNorm = error / 3500.0;

  // Integral controlado
  if (abs(error) < 1000) {
    integral += errorNorm;
  }

  integral = constrain(integral, -10, 10);

  float derivada = errorNorm - (lastError / 3500.0);

  float ajuste = kp * errorNorm + ki * integral + kd * derivada;

  lastError = error;

  int motorDer = velBas + (ajuste * velMax);
  int motorIzq = velBas - (ajuste * velMax);

  motorIzq = constrain(motorIzq, -velMax, velMax);
  motorDer = constrain(motorDer, -velMax, velMax);

  setMotor(motorIzq, motorDer);
}