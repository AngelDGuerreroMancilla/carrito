#include <Arduino.h>

void setup() {
  // Iniciamos la comunicación serie a 115200 baudios
  Serial.begin(115200);
  
  // Damos un pequeño respiro para darte tiempo de abrir el Monitor Serie
  delay(3000); 

  Serial.println("\n--- Test de Memoria YD-ESP32-S3 N16R8 ---");

  // 1. Verificar tamaño de memoria Flash (Donde se guarda el código)
  uint32_t flashSize = ESP.getFlashChipSize();
  Serial.print("Tamaño de Flash detectado: ");
  Serial.print(flashSize / (1024 * 1024));
  Serial.println(" MB");

  // 2. Verificar tamaño de memoria PSRAM (La RAM extra)
  uint32_t psramSize = ESP.getPsramSize();
  Serial.print("Tamaño de PSRAM detectado: ");
  if (psramSize > 0) {
    Serial.print(psramSize / (1024 * 1024));
    Serial.println(" MB");
  } else {
    Serial.println("0 MB (¡PSRAM no detectada! Revisa tu platformio.ini)");
  }

  // 3. Verificar la RAM interna libre
  Serial.print("RAM interna libre (SRAM): ");
  Serial.print(ESP.getFreeHeap() / 1024);
  Serial.println(" KB");
  
  Serial.println("-----------------------------------------");
}

void loop() {
  // No necesitamos hacer nada cíclico para esta prueba
  delay(10000);
}