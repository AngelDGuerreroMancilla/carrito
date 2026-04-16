#pragma once
#include <Arduino.h>
#include <PubSubClient.h>

extern PubSubClient client;
extern const char* mqttServer;

void setup_wifi();
void reconnect();
void envSig(String topic, String sig);