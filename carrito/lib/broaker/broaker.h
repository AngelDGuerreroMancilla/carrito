#ifndef BROAKER_H
#define BROAKER_H

#pragma once
#include <Arduino.h>
#include <PubSubClient.h>

extern PubSubClient client;
extern const char* mqttServer;
extern bool gpsIsActive;
void setup_wifi();
void reconnect();
void envSig(String topic, String sig);
#endif