#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>
#include <wifiudp.h>
#include <RemoteDebug.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <LittleFS.h>

RemoteDebug Debug;

const int led = 2;
int ledState = LOW;

bool GPIO_State = 0;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
String scanResults = "";
bool scanInProgress = false;
int lastScanMillis;

void notifyClients()
{
  debugD("notifyClients %D", String(GPIO_State));
  ws.textAll(String(GPIO_State));
}

void setup() {
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  Serial.begin(115200);

  delay(100);

  Serial.println("Booting NodeMCU Home Control");
  Serial.printf("Free heap at startup: %d\n", ESP.getFreeHeap());
  Serial.printf("Flash chip size: %d\n", ESP.getFlashChipSize());
  Serial.printf("Sketch size: %d\n", ESP.getSketchSize());

  connectToWifi();

  Serial.printf("Final WiFi status: %d\n", WiFi.status());
  Serial.printf("WiFi mode: %d\n", WiFi.getMode());
  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("Connected to: %s\n", WiFi.SSID().c_str());
    Serial.printf("IP: %s\n", WiFi.localIP().toString().c_str());
  } else if (WiFi.getMode() == WIFI_AP) {
    Serial.printf("AP mode - IP: %s\n", WiFi.softAPIP().toString().c_str());
  }

  Debug.begin("ESP8266");
  Debug.setSerialEnabled(true);

  if (!LittleFS.begin()) {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  Serial.printf("Free heap after LittleFS init: %d\n", ESP.getFreeHeap());

  listDir(LittleFS, "/", 0);

  initOTA();
  digitalWrite(led, LOW);
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  initWebSocket();

  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/index.html", "text/html"); });

  Serial.println("Setting up API endpoints...");
  String json = scanWifi();
  Serial.println("WiFi scan completed, JSON response ready: ");
  Serial.println(json);

  server.on("/api/scan", HTTP_GET, handleWifiScanRequest);
  server.on("/api/test", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Test endpoint called");
    request->send(200, "application/json", "{\"status\":\"ok\",\"message\":\"Server is working\"}");
  });

  server.on("/api/wifi", HTTP_POST, [](AsyncWebServerRequest *request){}, NULL,
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, (char*)data);

      String newSSID = doc["ssid"];
      String newPassword = doc["password"];

      File configFile = LittleFS.open("/wifi_config.json", "w");
      if (configFile) {
        String config = "{\"ssid\":\"" + newSSID + "\",\"password\":\"" + newPassword + "\"}";
        configFile.print(config);
        configFile.close();

        request->send(200, "application/json", "{\"status\":\"success\",\"message\":\"WiFi credentials saved\"}");
        delay(1000);
        Serial.println("Restarting to apply new WiFi credentials...");
        ESP.restart();
      } else {
        request->send(500, "application/json", "{\"status\":\"error\",\"message\":\"Failed to save credentials\"}");
      }
    });

  server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest *request){
    String json = "{";
    json += "\"connected\":" + String(WiFi.isConnected() ? "true" : "false") + ",";
    json += "\"ssid\":\"" + WiFi.SSID() + "\",";
    json += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
    json += "\"rssi\":" + String(WiFi.RSSI());
    json += "}";
    request->send(200, "application/json", json);
  });

  server.onNotFound([](AsyncWebServerRequest *request) {
    Serial.printf("404 Not Found: %s %s\n", request->methodToString(), request->url().c_str());
    Serial.printf("Client IP: %s\n", request->client()->remoteIP().toString().c_str());
    request->send(404, "text/plain", "Not Found");
 });

  Serial.println("Starting web server...");
  server.begin();
  Serial.println("Web server started successfully!");
  Serial.printf("Access the web interface at: http://%s\n", WiFi.localIP().toString().c_str());
}

void loop() {
  ArduinoOTA.handle();
  Debug.handle();
}
