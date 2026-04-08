#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <QTRSensors.h>
#include <Preferences.h>
#include <TinyGPS++.h>

#define echo 7
#define trig 42
#define drvIn1 15
#define drvIn2 10
#define drvIn3 18
#define drvIn4 46

#define lin1 4
#define lin2 5
#define lin3 6
#define lin4 8
#define lin5 3
#define lin6 9
#define lin7 1
#define lin8 2

#define RXD2 16
#define TXD2 17

QTRSensors qtr;
Preferences preferencias;

const uint8_t SensorCount = 8;
uint8_t pinesSensores[SensorCount] = {4, 5, 6, 8, 3, 9, 1, 2};





// unsigned long duracion;
int distancia;
long ultima_medicion=0;
bool modSegLin = false;
bool calibracion=0;



int lastError = 0;
float integral=0;

float kp = 0.1;  
float kd = 0.3;   
float ki = 0;
int velBas = 100; // Velocidad normal de los motores en línea recta (0-255)
int velMax = 120; // Límite de PWM


bool gpsIsActive= 0;


const char* ssid= "INFINITUM2059";
const char* password = "kzcSZ5TaC3";
const char* mqttServer = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient); 

TinyGPSPlus gps;


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
  analogWrite(drvIn1,120);
  analogWrite(drvIn2, 0);
  analogWrite(drvIn3,100);
  analogWrite(drvIn4,0); 
}
void movAtras(){
  analogWrite(drvIn1,0);
  analogWrite(drvIn2, 75);
  analogWrite(drvIn3,0);
  analogWrite(drvIn4,75); 
}
void movIzq(){
  analogWrite(drvIn1,0);
  analogWrite(drvIn2, 0);
  analogWrite(drvIn3,100);
  analogWrite(drvIn4,0); 
}
void movDer(){
  analogWrite(drvIn1,100);
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
void calibrar(){  
  Serial.println("\n*** INICIANDO CALIBRACION ***");
  Serial.println("¡MUEVE EL ROBOT DE LADO A LADO SOBRE LA LINEA!");
  
  // Leemos 400 veces (toma aprox. 10 segundos)
  for (uint16_t i = 0; i < 400; i++) {
    qtr.calibrate();
  }
  
  Serial.println("Calibracion terminada. Guardando en memoria...");

  
  for (uint8_t i = 0; i < SensorCount; i++) {
    preferencias.putUInt(("min" + String(i)).c_str(), qtr.calibrationOn.minimum[i]);
    preferencias.putUInt(("max" + String(i)).c_str(), qtr.calibrationOn.maximum[i]);
  }
  
  Serial.println("¡Datos guardados con éxito!");
}

void cargarCalibracion(){
  Serial.println("\nCargando calibracion desde memoria...");
  
  qtr.calibrate(); 

  for (uint8_t i = 0; i < SensorCount; i++) {
    // Si no hay datos, ponemos el mínimo en 1023 y el máximo en 1023 por seguridad
    qtr.calibrationOn.minimum[i] = preferencias.getUInt(("min" + String(i)).c_str(), 1023);
    qtr.calibrationOn.maximum[i] = preferencias.getUInt(("max" + String(i)).c_str(), 1023);
  }
  Serial.println("¡Calibracion cargada y lista para correr!");
}


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
    cargarCalibracion();
  }
  if(strcmp(topic,"mi_carrito/esp32/calibracion")==0){
    calibracion= msj.toInt();
    if (calibracion== 1){
      calibrar();
    }else{
      
    }
 
  }
  if(strcmp(topic, "mi_carrito/esp32/kp")== 0){
    kp= msj.toFloat();
    Serial.println("kp estableido a "); Serial.println(kp);
  }
  if(strcmp(topic, "mi_carrito/esp32/kd")== 0){
    kd= msj.toFloat();
    Serial.println("kd establecido a "); Serial.println(kd);
  }  
  if(strcmp(topic, "mi_carrito/esp32/ki")== 0){
    ki= msj.toFloat();
    Serial.println("ki establecido a "); Serial.println(ki);
  }
  if(strcmp(topic, "mi_carrito/esp32/velBas")== 0){
    velBas= msj.toInt();
    Serial.println("velBas establecido a "); Serial.println(velBas);
  }
  if(strcmp(topic, "mi_carrito/esp32/velMax")== 0){
    velMax= msj.toInt();
    Serial.println("velMax establecido a "); Serial.println(velMax);
  } 
  if(strcmp(topic, "mi_carrito/esp32/actGps")== 0){
    gpsIsActive= msj.toInt();
    Serial.print("gps "); Serial.println(msj.toInt());
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




void envSig(String topic, String sig){
    
    Serial.print("enviando a la web : ");Serial.print(topic); Serial.print(" ");
    Serial.println(sig);
    String topico="mi_carrito/web/";
    topico+= topic;
    client.publish(topico.c_str(), sig.c_str());
  
}
void envPos(){
  if(gps.location.isValid() && gps.location.isUpdated()){

    double lat = gps.location.lat();
    double lng = gps.location.lng();
        
    Serial.print("Latitud: "); Serial.println(lat, 6);
    Serial.print("Longitud: "); Serial.println(lng, 6);
        
    String latEnv = String(lat, 6);
    String lngEnv = String(lng, 6);
      
    envSig("lat", latEnv);
    envSig("lng", lngEnv);
  }
}
void estadoGps() {
 
  static long ultimo_estado_gps = 0;
  long ahora = millis();

  
  if (ahora - ultimo_estado_gps > 2000) {
    ultimo_estado_gps = ahora;
    if (!gps.location.isValid()) {
      Serial.print("Satélites a la vista: ");
      Serial.println(gps.satellites.value());
    } else {
      Serial.print("GPS: ¡Conectado! Satélites en uso: ");
      Serial.println(gps.satellites.value());
      
      // Si quieres mandar este aviso a tu web, descomenta estas líneas:
      String msj = String(gps.satellites.value());
      envSig("satel", msj);
    }
  }
}
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
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  delay(1000);  

  analogReadResolution(10);
  pinMode(echo, INPUT);
  pinMode(trig, OUTPUT);
  pinMode(drvIn1, OUTPUT);
  pinMode(drvIn2, OUTPUT);
  pinMode(drvIn3, OUTPUT);
  pinMode(drvIn4, OUTPUT);

 
  apagar();

  qtr.setTypeAnalog();
  qtr.setSensorPins(pinesSensores, SensorCount);
  preferencias.begin("datos_qtr", false);
  cargarCalibracion();
  setup_wifi();
  
  client.setServer(mqttServer, 1883);
  client.setCallback(recibirAlerta);
}


void loop(){
  if(!client.connected()){
    reconnect();
  }
  if(gpsIsActive) {
    while(Serial2.available() > 0) {
      if (gps.encode(Serial2.read())) {
       
        envPos(); 
      }
    }
  }
  estadoGps();

  if(modSegLin== 1){
      contrLineas();
  }
  client.loop();
  long ahora=millis();
  // if (ahora-ultima_medicion>50){
  //   ultima_medicion = ahora;
  //   // distCm();
    
   
  // }
  
}
