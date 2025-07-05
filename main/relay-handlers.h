#ifndef RELAY_HANDLERS_H
#define RELAY_HANDLERS_H

#include <ESPAsyncWebServer.h>

extern const int relay1Pin;
extern const int relay2Pin;

extern bool relay1State;
extern bool relay2State;

void handleRelayStatusRequest(AsyncWebServerRequest *request);
void handleRelay1ControlRequest(AsyncWebServerRequest *request);
void handleRelay2ControlRequest(AsyncWebServerRequest *request);

#endif
