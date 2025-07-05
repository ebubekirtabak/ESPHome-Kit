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
#include <DNSServer.h>
#include "wifi-handlers.h"
#include "build_info.h"

RemoteDebug Debug;
DNSServer dnsServer;

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

  Debug.begin("ESP8266");
  Debug.setSerialEnabled(true);
  delay(100);

  while (!Serial) {
    delay(100);
  }

  Serial.println("Booting NodeMCU Home Control");
  Serial.printf("Firmware Version: %s\n", FIRMWARE_VERSION);
  Serial.printf("Build Number: %d\n", BUILD_NUMBER);
  Serial.printf("Build Date: %s %s\n", BUILD_DATE, BUILD_TIME);
  Serial.printf("Free heap at startup: %d\n", ESP.getFreeHeap());
  Serial.printf("Flash chip size: %d\n", ESP.getFlashChipSize());
  Serial.printf("Sketch size: %d\n", ESP.getSketchSize());

  if (!LittleFS.begin()) {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  connectToWifi();

  Serial.printf("Final WiFi status: %d\n", WiFi.status());
  Serial.printf("WiFi mode: %d\n", WiFi.getMode());
  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("Connected to: %s\n", WiFi.SSID().c_str());
    Serial.printf("IP: %s\n", WiFi.localIP().toString().c_str());
  } else if (WiFi.getMode() == WIFI_AP) {
    Serial.printf("AP mode - IP: %s\n", WiFi.softAPIP().toString().c_str());
    const byte DNS_PORT = 53;
    dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
    Serial.println("DNS Server started for captive portal");
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

  // Client-side routes - serve index.html for React Router
  // Serve index.html for all client-side routes (except API/assets/static files)
  server.on("^\\/([a-zA-Z0-9\\-_/]+)?$", HTTP_GET, [](AsyncWebServerRequest *request) {
    String url = request->url();
    if (!url.startsWith("/api/") && !url.startsWith("/assets/") && url.indexOf('.') == -1) {
      request->send(LittleFS, "/index.html", "text/html");
    } else {
      request->send(404, "text/plain", "Not Found");
    }
  });

  server.on("/generate_204", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->redirect("http://" + WiFi.softAPIP().toString());
  });

  server.on("/connecttest.txt", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->redirect("http://" + WiFi.softAPIP().toString());
  });

  server.on("/hotspot-detect.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->redirect("http://" + WiFi.softAPIP().toString());
  });

  server.on("/canonical.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->redirect("http://" + WiFi.softAPIP().toString());
  });

  server.on("/success.txt", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->redirect("http://" + WiFi.softAPIP().toString());
  });

  server.on("/ncsi.txt", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->redirect("http://" + WiFi.softAPIP().toString());
  });

  Serial.println("Setting up API endpoints...");

  server.on("/api/scan", HTTP_GET, handleWifiScanRequest);
  server.on("/api/test", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Test endpoint called");
    request->send(200, "application/json", "{\"status\":\"ok\",\"message\":\"Server is working\"}");
  });

  server.on("/api/wifi_json", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("wifiConfig endpoint called");
    DynamicJsonDocument wifiConfig = loadWiFiConfig();
    if (!wifiConfig.isNull()) {
      Serial.println("Loaded WiFi config: " );
      String ssid = wifiConfig["ssid"] | "";
      Serial.println(ssid);
    }

    String configString;
    serializeJson(wifiConfig, configString);
    request->send(200, "application/json", configString);
  });

  server.on("/api/wifi", HTTP_POST, [](AsyncWebServerRequest *request){}, NULL, handleWifiRequest);

  server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest *request){
    String json = "{";
    json += "\"connected\":" + String(WiFi.isConnected() ? "true" : "false") + ",";
    json += "\"ssid\":\"" + WiFi.SSID() + "\",";
    json += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
    json += "\"rssi\":" + String(WiFi.RSSI()) + ",";
    json += "\"firmwareVersion\":\"" + String(FIRMWARE_VERSION) + "\",";
    json += "\"buildNumber\":" + String(BUILD_NUMBER) + ",";
    json += "\"buildDate\":\"" + String(BUILD_DATE) + "\",";
    json += "\"buildTime\":\"" + String(BUILD_TIME) + "\",";
    json += "\"freeHeap\":" + String(ESP.getFreeHeap()) + ",";
    json += "\"uptime\":" + String(millis()) + ",";
    json += "\"chipId\":\"" + String(ESP.getChipId(), HEX) + "\"";
    json += "}";
    request->send(200, "application/json", json);
  });

  server.onNotFound([](AsyncWebServerRequest *request) {
    Serial.printf("404 Not Found: %s %s\n", request->methodToString(), request->url().c_str());
    Serial.printf("Client IP: %s\n", request->client()->remoteIP().toString().c_str());

    String url = request->url();

    if (WiFi.getMode() == WIFI_AP || WiFi.getMode() == WIFI_AP_STA) {
      Serial.println("Redirecting to captive portal");
      String redirectHTML = "<!DOCTYPE html><html><head>";
      redirectHTML += "<meta charset='utf-8'>";
      redirectHTML += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
      redirectHTML += "<title>WiFi Setup</title>";
      redirectHTML += "<script>window.location.href='http://" + WiFi.softAPIP().toString() + "/';</script>";
      redirectHTML += "</head><body>";
      redirectHTML += "<p>Redirecting to WiFi setup...</p>";
      redirectHTML += "<p>If not redirected automatically, <a href='http://" + WiFi.softAPIP().toString() + "/'>click here</a></p>";
      redirectHTML += "</body></html>";
      request->send(200, "text/html", redirectHTML);
    } else if (!url.startsWith("/api/") && !url.startsWith("/assets/") &&
      url.indexOf('.') == -1 && request->method() == HTTP_GET) {
      Serial.println("Serving index.html for client-side route: " + url);
      request->send(LittleFS, "/index.html", "text/html");
    } else {
      request->send(404, "text/plain", "Not Found");
    }
 });

  server.on("/test", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Test page requested");
    String testHTML = R"html(
      <!DOCTYPE html>
      <html><head><title>Captive Portal Test</title></head>
      <body>
      <h1>NodeMCU Captive Portal Test</h1>
      <p>If you can see this page, the captive portal is working!</p>
      <p>Device IP: )html" + (WiFi.getMode() == WIFI_AP ? WiFi.softAPIP().toString() : WiFi.localIP().toString()) + R"html(</p>
      <p>WiFi Mode: )html" + String(WiFi.getMode()) + R"html(</p>
      <p><a href="/">Go to main page</a></p>
      </body></html>
      )html";
    request->send(200, "text/html", testHTML);
  });

  Serial.println("Starting web server...");
  server.begin();
  Serial.println("Web server started successfully!");
  Serial.printf("Access the web interface at: http://%s\n", WiFi.localIP().toString().c_str());
}

void loop() {
  ArduinoOTA.handle();
  Debug.handle();

  if (WiFi.getMode() == WIFI_AP || WiFi.getMode() == WIFI_AP_STA) {
    dnsServer.processNextRequest();
  }
}
