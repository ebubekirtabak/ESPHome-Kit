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

void notifyClients()
{
  debugD("notifyClients %D", String(GPIO_State));
  ws.textAll(String(GPIO_State));
}


void listDir(fs::FS &fs, const char *dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\n", dirname);
  File root = fs.open(dirname, "r");
  if (!root) {
    Serial.println("- failed to open directory");
    return;
  }

  if (!root.isDirectory()) {
    Serial.println(" - not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        String path = String(dirname);
        if (!path.endsWith("/")) path += "/";
        path += file.name();
        listDir(fs, path.c_str(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }

    file = root.openNextFile();
  }
}

void setup() {
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  Serial.begin(115200);

  // Wait for serial to be ready
  delay(100);

  Serial.println("Booting NodeMCU Home Control");
  Serial.printf("Free heap at startup: %d\n", ESP.getFreeHeap());
  Serial.printf("Flash chip size: %d\n", ESP.getFlashChipSize());
  Serial.printf("Sketch size: %d\n", ESP.getSketchSize());

  connectToWifi();

  // Print final WiFi status
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

  // Initialize LittleFS
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
            { request->send(LittleFS, "/index.html", "text/html"); });  // API endpoint to scan WiFi networks
  server.on("/api/scan", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("=== WiFi scan requested ===");
    Serial.printf("Free heap before scan: %d\n", ESP.getFreeHeap());
    Serial.printf("WiFi mode: %d, Status: %d\n", WiFi.getMode(), WiFi.status());

    // Force WiFi mode if needed
    if (WiFi.getMode() == WIFI_OFF) {
      Serial.println("WiFi was off, setting to STA mode");
      WiFi.mode(WIFI_STA);
      delay(500);
    }

    Serial.println("Starting WiFi scan...");
    yield();

    // Try a very simple scan first
    int n = WiFi.scanNetworks(false, false); // No hidden networks, no async
    Serial.printf("Scan completed, result: %d\n", n);
    yield();

    if (n == -1) {
      Serial.println("Scan already in progress");
      request->send(202, "application/json", "{\"message\":\"Scan in progress, try again soon.\"}");
      return;
    } else if (n == -2) {
      Serial.println("Scan failed");
      request->send(500, "application/json", "{\"error\":\"WiFi scan failed. Try again.\"}");
      return;
    } else if (n == 0) {
      Serial.println("No networks found - sending empty array");
      request->send(200, "application/json", "[]");
      return;
    }

    Serial.printf("Processing %d networks...\n", n);
    Serial.printf("Free heap after scan: %d\n", ESP.getFreeHeap());

    int maxNetworks = min(n, 3);
    Serial.printf("Will process %d networks (limited from %d)\n", maxNetworks, n);

    String json = "";
    json.reserve(500);
    json = "[";

    for (int i = 0; i < maxNetworks; i++) {
      Serial.printf("Processing network %d...\n", i);
      yield();

      if (i > 0) json += ",";

      String ssid = "";
      int rssi = 0;
      bool secure = false;

      ssid = WiFi.SSID(i);
      rssi = WiFi.RSSI(i);
      secure = (WiFi.encryptionType(i) != ENC_TYPE_NONE);

      Serial.printf("Network %d: SSID='%s', RSSI=%d, Secure=%s\n",
                   i, ssid.c_str(), rssi, secure ? "true" : "false");

      if (ssid.length() == 0 && rssi == 0) {
        Serial.printf("Invalid data for network %d, skipping\n", i);
        continue;
      }

      if (ssid.length() == 0) {
        ssid = "Hidden";
      }

      // Simple character replacement
      ssid.replace("\"", "'");
      ssid.replace("\\", "/");

      json += "{\"ssid\":\"" + ssid + "\",";
      json += "\"rssi\":" + String(rssi) + ",";
      json += "\"secure\":" + String(secure ? "true" : "false") + "}";

      yield();
    }
    json += "]";

    Serial.printf("JSON built, length: %d\n", json.length());
    Serial.printf("Free heap before response: %d\n", ESP.getFreeHeap());

    WiFi.scanDelete();
    yield();

    Serial.println("Sending response...");
    request->send(200, "application/json", json);
    Serial.println("=== Scan response sent ===");
  });

  server.on("/api/test", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Test endpoint called");
    request->send(200, "application/json", "{\"status\":\"ok\",\"message\":\"Server is working\"}");
  });

  server.on("/api/scan-simple", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Simple WiFi scan requested");

    ESP.wdtFeed();

    int n = WiFi.scanNetworks();
    if (n <= 0) {
      request->send(200, "application/json", "[]");
      return;
    }

    int maxNetworks = min(n, 10);

    String json = "[";
    for (int i = 0; i < maxNetworks; i++) {
      if (i > 0) json += ",";
      String ssid = WiFi.SSID(i);
      ssid.replace("\"", "\\\"");
      json += "{\"ssid\":\"" + ssid + "\",";
      json += "\"rssi\":" + String(WiFi.RSSI(i)) + ",";
      json += "\"secure\":" + String(WiFi.encryptionType(i) != ENC_TYPE_NONE ? "true" : "false") + "}";

      if (i % 3 == 0) yield();
    }
    json += "]";

    WiFi.scanDelete();
    request->send(200, "application/json", json);
    Serial.printf("Simple scan sent %d networks\n", maxNetworks);
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

        // Restart to connect with new credentials
        delay(1000);
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
    Serial.printf("404 Not Found: %s\n", request->url().c_str());
    request->send(404, "text/plain", "Not Found");
 });

  server.begin();
}

void loop() {
  // Feed watchdog timer first
  ESP.wdtFeed();

  ArduinoOTA.handle();
  Debug.handle();

  static unsigned long lastMemoryCheck = 0;
  if (millis() - lastMemoryCheck > 30000) {
    Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
    lastMemoryCheck = millis();

    // Force garbage collection if memory is low
    if (ESP.getFreeHeap() < 5000) {
      Serial.println("Low memory detected, forcing garbage collection");
      ESP.wdtFeed();
    }
  }

  yield();
}
