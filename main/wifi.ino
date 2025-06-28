#ifndef STASSID
#define STASSID "SUPERONLINE_WiFi_C9A3"
#define STAPSK "Sparta_21?"
#endif

const char *defaultSSID = STASSID;
const char *defaultPassword = STAPSK;
IPAddress local_IP(192, 168, 1, 184);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

String loadWiFiConfig() {
  if (LittleFS.exists("/wifi_config.json")) {
    File configFile = LittleFS.open("/wifi_config.json", "r");
    if (configFile) {
      String config = configFile.readString();
      configFile.close();
      return config;
    }
  }
  return "";
}


void connectToWifi() {
    WiFi.begin(defaultSSID, defaultPassword);
    if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
      Serial.println("STA Failed to configure");
    }

    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.println("Connection Failed! Rebooting...");
      digitalWrite(led, HIGH);
      // WiFi.disconnect();
      delay(900);
      // ESP.restart();
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

String scanWifi() {
    Serial.println("Starting WiFi scan...");
    int n = WiFi.scanNetworks();
    if (n <= 0) {
        Serial.printf("No networks found or scan failed: %d", n);
        WiFi.scanDelete();
      return "[]";
    }

    int maxNetworks = min(n, 10);

    String json = "[";
    for (int i = 0; i < maxNetworks; i++) {
      Serial.printf("Network %d: SSID='%s', RSSI=%d, Secure=%s\n",
                     i, WiFi.SSID(i).c_str(), WiFi.RSSI(i),
                     (WiFi.encryptionType(i) != ENC_TYPE_NONE) ? "true" : "false");
      if (i > 0) json += ",";
      String ssid = WiFi.SSID(i);
      ssid.replace("\"", "\\\"");
      json += "{\"ssid\":\"" + ssid + "\",";
      json += "\"rssi\":" + String(WiFi.RSSI(i)) + ",";
      json += "\"secure\":" + String(WiFi.encryptionType(i) != ENC_TYPE_NONE ? "true" : "false") + "}";
      delay(1);
      if (i % 3 == 0) yield();
    }
    json += "]";

    WiFi.scanDelete();
    return json;
}

void handleWifiScanRequest(AsyncWebServerRequest *request) {
  if (!scanInProgress && (millis() - lastScanMillis > 30000)) {
    scanInProgress = true;
    scanResults = "";
    WiFi.scanNetworksAsync([&](int networksFound) {
      DynamicJsonDocument doc(2048);

      JsonArray arr = doc.createNestedArray("networks");
      for (int i = 0; i < networksFound; i++) {
        JsonObject net = arr.createNestedObject();
        net["ssid"] = WiFi.SSID(i);
        net["rssi"] = WiFi.RSSI(i);
        net["bssid"] = WiFi.BSSIDstr(i);
        net["channel"] = WiFi.channel(i);
        net["encryptionType"] = WiFi.encryptionType(i);
      }

      doc["status"] = "scan_completed";
      serializeJson(doc, scanResults);
      Serial.printf("Scan completed, found %d networks\n", networksFound);
      scanInProgress = false;
      lastScanMillis = millis();
    });
    request->send(200, "application/json", "{\"status\":\"scan_started\"}");
  } else {
    if (scanResults.isEmpty()) {
      request->send(200, "application/json", "{\"status\":\"scan_in_progress\"}");
    } else {
      request->send(200, "application/json", scanResults);
    }
  }
}
