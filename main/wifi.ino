IPAddress local_IP(192, 168, 1, 184);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

DynamicJsonDocument loadWiFiConfig() {
  if (LittleFS.exists("wifi_config.json")) {
    File configFile = LittleFS.open("wifi_config.json", "r");
    if (configFile) {
      String config = configFile.readString();
      configFile.close();
      DynamicJsonDocument doc(512);
      DeserializationError error = deserializeJson(doc, config);
      if (!error) {
        debugV("wifiConfig: %s", config.c_str());
        return doc;
      } else {
        debugV("Failed to parse WiFi config JSON.");
        return DynamicJsonDocument(512);
      }
    }
  }

  Serial.println("WiFi config file not found, using default credentials.");
  return DynamicJsonDocument(512);
}

void connectToWifi() {
  DynamicJsonDocument wifiConfig = loadWiFiConfig();
  if (!wifiConfig.isNull() && wifiConfig.containsKey("ssid") && wifiConfig.containsKey("password")) {
    String ssid = wifiConfig["ssid"] | "";
    String password = wifiConfig["password"] | "";
    Serial.printf("Loaded WiFi config: %s %s\n", ssid.c_str(), password.c_str());
    WiFi.begin(ssid, password);
    if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
      Serial.println("STA Failed to configure");
    }

    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.println("Connection Failed! Rebooting...");
      digitalWrite(led, HIGH);
      WiFi.disconnect();
      delay(200);
      createAP();
      return;
    }

    debugV("WiFi connected: %s", WiFi.localIP().toString().c_str());
  } else {
    debugV("No WiFi config found, starting AP mode.");
    createAP();
  }
}

void createAP() {
  const char *ap_ssid = "NodeMCU_Config";
  const char *ap_password = "";

  WiFi.mode(WIFI_AP);

  IPAddress apIP(192, 168, 4, 1);
  IPAddress apGateway(192, 168, 4, 1);
  IPAddress apSubnet(255, 255, 255, 0);

  WiFi.softAPConfig(apIP, apGateway, apSubnet);

  if (WiFi.softAP(ap_ssid, ap_password)) {
    IPAddress actualIP = WiFi.softAPIP();
    debugV("AP mode active. Connect to SSID '%s' to configure WiFi.", ap_ssid);
    debugV("Captive portal available at: http://%s", actualIP.toString().c_str());
  } else {
    Serial.println("Failed to start Access Point!");
  }
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
