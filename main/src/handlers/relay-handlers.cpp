#include "../../relay-handlers.h"
#include <ESPAsyncWebServer.h>
#include <Arduino.h>
#include <RemoteDebug.h>

void handleRelayStatusRequest(AsyncWebServerRequest *request) {
  String json = "{";
  json += "\"relay1State\":" + String(relay1State ? "true" : "false") + ",";
  json += "\"relay2State\":" + String(relay2State ? "true" : "false");
  json += "}";
  request->send(200, "application/json", json);
}

void handleRelay1ControlRequest(AsyncWebServerRequest *request) {
  if (request->hasParam("state", true)) {
    String state = request->getParam("state", true)->value();
    relay1State = (state == "on" || state == "true" || state == "1");
    digitalWrite(relay1Pin, relay1State ? HIGH : LOW);

    debugD("Relay 1 turned %s\n", relay1State ? "ON" : "OFF");

    String json = "{\"status\":\"success\",\"relay1State\":" + String(relay1State ? "true" : "false") + "}";
    request->send(200, "application/json", json);
  } else {
    request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing state parameter\"}");
  }
}

void handleRelay2ControlRequest(AsyncWebServerRequest *request) {
  if (request->hasParam("state", true)) {
    String state = request->getParam("state", true)->value();
    relay2State = (state == "on" || state == "true" || state == "1");
    digitalWrite(relay2Pin, relay2State ? HIGH : LOW);

    debugD("Relay 2 turned %s\n", relay2State ? "ON" : "OFF");

    String json = "{\"status\":\"success\",\"relay2State\":" + String(relay2State ? "true" : "false") + "}";
    request->send(200, "application/json", json);
  } else {
    request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing state parameter\"}");
  }
}
