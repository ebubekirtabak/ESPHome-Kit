#ifndef WIFI_HANDLERS_H
#define WIFI_HANDLERS_H

#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

void handleWifiRequest(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);

#endif
