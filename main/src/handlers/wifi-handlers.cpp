#include "../../wifi-handlers.h"
#include <ESP8266WiFi.h>

void handleWifiRequest(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
  DynamicJsonDocument inputDoc(1024);
  deserializeJson(inputDoc, (char*)data);

  String newSSID = inputDoc["ssid"];
  String newPassword = inputDoc["password"];

  DynamicJsonDocument configDoc(1024);

  if (LittleFS.exists("/wifi_config.json")) {
    Serial.println("Loading existing WiFi config to preserve other fields...");
    File existingFile = LittleFS.open("/wifi_config.json", "r");
    if (existingFile) {
      String existingConfig = existingFile.readString();
      existingFile.close();

      DeserializationError error = deserializeJson(configDoc, existingConfig);
      if (error) {
        Serial.println("Failed to parse existing config, creating new one");
        configDoc.clear();
      } else {
        Serial.println("Successfully loaded existing config");
      }
    }
  }

  configDoc["ssid"] = newSSID;
  configDoc["password"] = newPassword;

  if (!configDoc.containsKey("local_IP")) {
    configDoc["local_IP"] = "192.168.1.184";
  }

  if (LittleFS.exists("/wifi_config.json")) {
    LittleFS.remove("/wifi_config.json");
  }

  File configFile = LittleFS.open("/wifi_config.json", "w");
  if (configFile) {
    String configString;
    serializeJson(configDoc, configString);
    configFile.print(configString);
    configFile.close();

    Serial.printf("WiFi config saved: %s\n", configString.c_str());
    request->send(200, "application/json", "{\"status\":\"success\",\"message\":\"WiFi credentials saved\"}");
    delay(1000);
    Serial.println("Restarting to apply new WiFi credentials...");
    ESP.restart();
  } else {
    request->send(500, "application/json", "{\"status\":\"error\",\"message\":\"Failed to save credentials\"}");
  }
}
