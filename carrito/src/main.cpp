#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#define echo 4
#define trig 5

#define drvIn1 6
#define drvIn2 7
#define drvIn3 15
#define drvIn4 16

unsigned long duracion;
int distancia;
long ultima_medicion=0;

const char* ssid= "INFINITUM2059";
const char* password = "kzcSZ5TaC3";
const char* mqttServer = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient); 




void controlSensor( int distSensUlt){
  if (distSensUlt <= 20){
    digitalWrite(drvIn1, HIGH);
    digitalWrite(drvIn2, HIGH);
    digitalWrite(drvIn3, HIGH);
    digitalWrite(drvIn4, HIGH);
    
  }else{
    digitalWrite(drvIn2, LOW);
    analogWrite(drvIn1, 2000); //4095 velocidad max
    digitalWrite(drvIn4, LOW);
    analogWrite(drvIn3, 2000); //4095 velocidad max
  }
  
}

//escucha todos los mensajes que llegan del broaker
//aqui llegan los topicos, dependiendo de los topicos vamos a ejecutar las funciones correspondientes.
void recibirAlerta(char* topic, byte* payload, unsigned int length){
  Serial.print("mensaje del tema : ");
  Serial.println(topic);
  String msjSensUlt = "";
  
  for(int i = 0 ; i< length; i++){
    msjSensUlt += (char)payload[i];
  }
  if (strcmp(topic, "mi_carrito/esp32/sensor") == 0) {
    int distSensUlt= msjSensUlt.toInt();
    controlSensor(distSensUlt);
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


void envSig(String topic, String sig){
    
    Serial.print("enviando duracion: ");
    Serial.println(sig);
    String topico="mi_carrito/web/";
    topico+= topic;
    client.publish(topico.c_str(), sig.c_str());
  
}
void envSensUlt(){ 
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  duracion = pulseIn(echo, HIGH,30000);
  if(duracion == 0 ){
    duracion = 500;
  }
  char durString[8];

  itoa(duracion, durString, 10);
  envSig("sensor", durString);
}


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
}


void loop(){
  if(!client.connected()){
    reconnect();
  }
  client.loop();
  long ahora=millis();
  if (ahora-ultima_medicion>1000){
    ultima_medicion = ahora;
    envSensUlt();
  }
}

