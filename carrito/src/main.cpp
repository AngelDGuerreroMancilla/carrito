#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <sensor.h>
#define echo 4
#define trig 5

#define drvIn1 6
#define drvIn2 7
#define drvIn3 15
#define drvIn4 16

#define lin1 17
#define lin2 18
#define lin3 8
#define lin4 3
#define lin5 46



unsigned long duracion;
int distancia;
long ultima_medicion=0;
bool modSegLin = false;


const char* ssid= "INFINITUM2059";
const char* password = "kzcSZ5TaC3";
const char* mqttServer = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient); 


// void controlSensor( int distSensUlt){
//   if (distSensUlt <= 20){
    
//     analogWrite(drvIn1, 0); 
//     digitalWrite(drvIn2, LOW);
    
//     analogWrite(drvIn3, 0);
//     digitalWrite(drvIn4, LOW);
    
//     Serial.println("Freno Activado (Motores apagados)");
    
//   }else{
    
//     digitalWrite(drvIn2, LOW);
//     analogWrite(drvIn1, 200); // velocidad 
    
//     analogWrite(drvIn3, 200); // velocidad 
    
//     Serial.println("Avanzando");
//   }
// }


void movDel(){
  analogWrite(drvIn1,200);
  analogWrite(drvIn2, 0);
  analogWrite(drvIn3,200);
  analogWrite(drvIn4,0); 
}
void movAtras(){
  analogWrite(drvIn1,0);
  analogWrite(drvIn2, 200);
  analogWrite(drvIn3,0);
  analogWrite(drvIn4,200); 
}
void movIzq(){
  analogWrite(drvIn1,0);
  analogWrite(drvIn2, 0);
  analogWrite(drvIn3,200);
  analogWrite(drvIn4,0); 
}
void movDer(){
  analogWrite(drvIn1,200);
  analogWrite(drvIn2, 0);
  analogWrite(drvIn3,0);
  analogWrite(drvIn4,0); 
}
void movDelDer(){
  analogWrite(drvIn1,200);
  analogWrite(drvIn2, 0);
  analogWrite(drvIn3,100);
  analogWrite(drvIn4,0); 

}
void movDelIzq(){
  analogWrite(drvIn1,100);
  analogWrite(drvIn2, 0);
  analogWrite(drvIn3,200);
  analogWrite(drvIn4,0); 

}


void contrlJoy(int ejeX, int ejeY){
  int velDer= ejeX +ejeY;
  int velIzq=ejeY-ejeX;

  velIzq= constrain(velIzq, -500,500);
  velDer = constrain(velDer, -500, 500);

  int pwmIzq = map(abs(velIzq), 0, 500, 0, 3000);
  int pwmDer = map(abs(velDer), 0, 500, 0, 3000);


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


}

void coordenadas(String mensaje){
  int indiceComa = mensaje.indexOf(",");
  if(indiceComa>0){
    String txtX= mensaje.substring(0,indiceComa);
    String txtY= mensaje.substring(indiceComa +1 );

    int ejeX = txtX.toInt();
    int ejeY = txtY.toInt();

    contrlJoy(ejeX, ejeY);

  }
}
void contrLineas(){
  bool extIzq = digitalRead(lin1);
  bool midIzq = digitalRead(lin2);
  bool mid = digitalRead(lin3);
  bool midDer= digitalRead(lin4);
  bool extDer= digitalRead(lin5);

  
  if(extIzq && !midIzq &&  !mid && !midDer && !extDer) 
  movIzq();
  if(!extIzq && midIzq &&  !mid && !midDer && !extDer) 
  movDelIzq();
  if(!extIzq && !midIzq &&  mid && !midDer && !extDer) 
  movDel();
  if(!extIzq && !midIzq &&  !mid && midDer && !extDer) 
  movDelDer();
  if(!extIzq && !midIzq &&  !mid && !midDer && extDer) 
  movDer();
  
}


//escucha todos los mensajes que llegan del broaker
//aqui llegan los topicos, dependiendo de los topicos vamos a ejecutar las funciones correspondientes.
void recibirAlerta(char* topic, byte* payload, unsigned int length){
  Serial.print("mensaje del tema : ");
  Serial.println(topic);
  String msj = "";
  
  for(int i = 0 ; i< length; i++){
    msj += (char)payload[i];
  }
  if (strcmp(topic, "mi_carrito/esp32/joystick") == 0) { 
    coordenadas( msj);
    
  }
  if(strcmp(topic,"mi_carrito/esp32/seguidorLineas" )== 0){
    modSegLin = msj.toInt();
  }
}




// se conecta a la red declarada anteriormente
void setup_wifi(){
  delay(10);
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado.");
}

//crea un id de cliente del broacker de ESP32TeamRC mas numeros random 
//quedando algo como ESP32TeamRC-A1B2
// se conecta al broaker y si se desconecta se reconecta
void reconnect(){
  while(!client.connected()){
      Serial.print("intentando volver a conectar a MQTT");
      String clientId = "ESP32TeamRC";
      clientId +=String(random(0,0xffff), HEX);
    
    if(client.connect(clientId.c_str())){
      Serial.println("conectao");
      client.subscribe("mi_carrito/esp32/#");
    }else {
      Serial.print("fallo , rc= ");
      Serial.print(client.state());
      delay(5000);
    }
  }
}


// void envSig(String topic, String sig){
    
//     Serial.print("enviando duracion: ");
//     Serial.println(sig);
//     String topico="mi_carrito/web/";
//     topico+= topic;
//     client.publish(topico.c_str(), sig.c_str());
  
// }
// void distCm(){ 
//   digitalWrite(trig, LOW);
//   delayMicroseconds(2);
//   digitalWrite(trig, HIGH);
//   delayMicroseconds(10);
//   digitalWrite(trig, LOW);
//   duracion = pulseIn(echo, HIGH,30000);
  
//   int distCm = (duracion * 0.0343/ 2 );
//   if(duracion == 0 ){
//     distCm = 500;
//   }
//   controlSensor(distCm);
  
// }


void setup(){
  pinMode(echo, INPUT);
  pinMode(trig, OUTPUT);
  pinMode(drvIn1, OUTPUT);
  pinMode(drvIn2, OUTPUT);
  pinMode(drvIn3, OUTPUT);
  pinMode(drvIn4, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqttServer, 1883);
  client.setCallback(recibirAlerta);
  analogWriteResolution(10);
}


void loop(){
  if(!client.connected()){
    reconnect();
  }
  client.loop();
  long ahora=millis();
  if (ahora-ultima_medicion>100){
    ultima_medicion = ahora;
    // distCm();
    if(modSegLin== 1){
      contrLineas();
    }
  }
}
