#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#define echo 4
#define trig 5
unsigned long duracion;
int distancia;
long ultima_medicion=0;

const char* ssid= "INFINITUM2059";
const char* password = "kzcSZ5TaC3";
const char* mqttServer = "broker.hivemq.com";

WiFiClient espClient; // duda
PubSubClient client(espClient); // duda
void recibirAlerta(char* topic, byte* payload, unsigned int length){// duda en los parametros
  Serial.print("mensaje del tema : ");
  Serial.println(topic);
  String mensaje = "";
  
  for(int i = 0 ; i< length; i++){
    mensaje += (char)payload[i];
  }
  Serial.print(mensaje);
  if (mensaje == "DETENER"){
    Serial.println(" hey you cant park there");
  }
}
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
void reconnect(){// duda
  while(!client.connected()){// duda
      Serial.print("intentando volver a conectar a MQTT");
      String clientId = "ESP32TeamRC";// duda
      clientId +=String(random(0,0xffff), HEX);// duda
    
    if(client.connect(clientId.c_str())){// duda
      Serial.println("conectao");
      client.subscribe("mi_carrito/alerta");// duda
    }else {
      Serial.print("fallo , rc= ");// duda
      Serial.print(client.state());// duda
      delay(5000);
    }
  }
}
int distCm(){
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  duracion = pulseIn(echo, HIGH,30000);
  // Cálculo de la distancia
  distancia = (duracion * 0.034) / 2;
  if (distancia== 0){
    distancia = 500;
  }
  return distancia;
}
void setup(){
  pinMode(echo, INPUT);
  pinMode(trig, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqttServer, 1883);//duda
  client.setCallback(recibirAlerta);//duda
}
void loop(){
  if(!client.connected()){
    reconnect();
  }
  client.loop();//duda
  long ahora=millis();//duda
  if (ahora-ultima_medicion>1000){//duda
    ultima_medicion = ahora;//duda
    int distancia_actual= distCm();
    Serial.print("enviando distancia: ");
    Serial.println(distancia_actual);
    char distString[8];//duda
    itoa(distancia_actual, distString, 10);//duda
    client.publish("mi_carrito/distancia", distString);//duda
  }
}

