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
  analogWrite(drvIn1,50);
  analogWrite(drvIn2, 0);
  analogWrite(drvIn3,50);
  analogWrite(drvIn4,0); 
}
// void movAtras(){
//   analogWrite(drvIn1,0);
//   analogWrite(drvIn2, 75);
//   analogWrite(drvIn3,0);
//   analogWrite(drvIn4,75); 
// }
void movIzq(){
  analogWrite(drvIn1,0);
  analogWrite(drvIn2, 0);
  analogWrite(drvIn3,50);
  analogWrite(drvIn4,0); 
}
void movDer(){
  analogWrite(drvIn1,50);
  analogWrite(drvIn2, 0);
  analogWrite(drvIn3,0);
  analogWrite(drvIn4,0); 
}
// void movDelDer(){
//   analogWrite(drvIn1,100);
//   analogWrite(drvIn2, 0);
//   analogWrite(drvIn3,50);
//   analogWrite(drvIn4,0); 

// // }
// void movDelIzq(){
//   analogWrite(drvIn1,50);
//   analogWrite(drvIn2, 0);
//   analogWrite(drvIn3,100);
//   analogWrite(drvIn4,0); 

// }
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
  int izq = digitalRead(lin1);
  int mid = digitalRead(lin2);
  int der = digitalRead(lin3);


  
  if(mid ==1 ) {
    movDel();
    Serial.println("medio");
  }
 
  else if(izq ==1) {
    movIzq();
    Serial.println("izq");
  }
  
  else if(der == 1 ) {
    movDer();
    Serial.println("der");
  }else{
    
    apagar();
    Serial.println("apagado");
  }
  
  
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
  Serial.flush();
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  int intentos = 0;
  int max_intentos = 20; // 10 segundos max
  
  while (WiFi.status() != WL_CONNECTED && intentos < max_intentos) {
    delay(500);
    Serial.print(".");
    Serial.flush();
    intentos++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi conectado.");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nNo se conectó a WiFi - continuando sin red");
  }
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
  pinMode(lin1, INPUT);
  pinMode(lin2, INPUT);
  pinMode(lin3, INPUT);
 
  Serial.begin(115200);
  
  delay(2000);
  Serial.println("¡Consola Serial funcionando!");
  setup_wifi();
  
  client.setServer(mqttServer, 1883);
  client.setCallback(recibirAlerta);
}


void loop(){
  if(!client.connected()){
    reconnect();
  }

  if(modSegLin== 1){
      contrLineas();
  }
  client.loop();
  long ahora=millis();
  if (ahora-ultima_medicion>100){
    ultima_medicion = ahora;
    // distCm();
   
  }
}
