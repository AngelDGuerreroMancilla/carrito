#ifndef motores_h
#define motores_h
#include "motores.h"
#include "leds.h"
volatile unsigned int pulsosIzq = 0;
volatile unsigned int pulsosDer = 0;
int velocidadBase = 150;
int pwmI = velocidadBase;
int pwmD = velocidadBase;



void movDel(){
  analogWrite(drvIn1,3000);
  analogWrite(drvIn2, 0);
  analogWrite(drvIn3,3000);
  analogWrite(drvIn4,0); 
}
void movAtras(){
  analogWrite(drvIn1,0);
  analogWrite(drvIn2, 100);
  analogWrite(drvIn3,0);
  analogWrite(drvIn4,100); 
}
void movIzq(){
  analogWrite(drvIn1,0);
  analogWrite(drvIn2, 0);
  analogWrite(drvIn3,3000);
  analogWrite(drvIn4,0); 
}
void movDer(){
  analogWrite(drvIn1,3000);
  analogWrite(drvIn2, 0);
  analogWrite(drvIn3,0);
  analogWrite(drvIn4,0); 
}
void movDelDer(){
  analogWrite(drvIn1,300);
  analogWrite(drvIn2, 0);
  analogWrite(drvIn3,100);
  analogWrite(drvIn4,0); 

}
void movDelIzq(){
  analogWrite(drvIn1,100);
  analogWrite(drvIn2, 0);
  analogWrite(drvIn3,300);
  analogWrite(drvIn4,0); 

}
void apagar(){
 
  analogWrite(drvIn1, 0);
  analogWrite(drvIn2, 0); 
  
 
  analogWrite(drvIn3, 0);
  analogWrite(drvIn4, 0); 
}

void contrlJoy(int ejeX, int ejeY){
  int velDer= ejeX +ejeY;
  int velIzq=ejeY-ejeX;

  velIzq= constrain(velIzq, -500,500);
  velDer = constrain(velDer, -500, 500);

  int pwmIzq = map(abs(velIzq), 0, 500, 0, 2500);
  int pwmDer = map(abs(velDer), 0, 500, 0, 2500);


  if (velIzq>10){
    analogWrite(drvIn1, pwmIzq);
    analogWrite(drvIn2, 0);
    
  }else if (velIzq < -10 ){
    analogWrite(drvIn1, 0);
    analogWrite(drvIn2, pwmIzq);

  }else{
    analogWrite(drvIn1, 0);
    analogWrite(drvIn2, 0);
  }

  if (velDer>10){
    analogWrite(drvIn3, pwmDer);
    analogWrite(drvIn4, 0);
    
  }else if (velDer < -10 ){
    analogWrite(drvIn3, 0);
    analogWrite(drvIn4, pwmDer);

  }else{
    analogWrite(drvIn3, 0);
    analogWrite(drvIn4, 0);
  }
  
   controlarDireccionales(ejeX);

}


void setMotor(int izq, int der) {

  // Motor izquierdo
  if (izq >= 0) {
    analogWrite(drvIn1, izq);
    analogWrite(drvIn2, 0);
  } else {
    analogWrite(drvIn1, 0);
    analogWrite(drvIn2, -izq);
  }

  // Motor derecho
  if (der >= 0) {
    analogWrite(drvIn3, der);
    analogWrite(drvIn4, 0);
  } else {
    analogWrite(drvIn3, 0);
    analogWrite(drvIn4, -der);
  }
} 

void seguirUbicacion(double cursoGrados, double destGrados){

}
#endif