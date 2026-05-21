#include <PubSubClient.h>
#include "broaker.h"
#include <WiFi.h>

WiFiClient espClient;
PubSubClient client(espClient); 

/* const char* ssid= "Mega_2.4G_424B";
const char* password = "k6tXAfxf"; */
const char* ssid= "Hiram";
const char* password = "12345678";
const char* mqttServer = "broker.hivemq.com";




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

void envSig(String topic, String sig){
    
    Serial.print("enviando a la web : ");Serial.print(topic); Serial.print(" ");
    Serial.println(sig);
    String topico="mi_carrito/web/";
    topico+= topic;
    client.publish(topico.c_str(), sig.c_str());
  
}
