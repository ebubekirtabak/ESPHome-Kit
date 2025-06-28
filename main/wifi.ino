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
    String config = loadWiFiConfig();
    String ssid = defaultSSID;
    String password = defaultPassword;

    if (config.length() > 0) {
      DynamicJsonDocument doc(512);
      DeserializationError error = deserializeJson(doc, config);
      if (!error) {
        ssid = doc["ssid"].as<String>();
        password = doc["password"].as<String>();
        Serial.println("Using saved WiFi credentials");
        Serial.println("SSID: " + ssid);
      } else {
        Serial.println("Failed to parse saved WiFi config, using defaults");
      }
    } else {
      Serial.println("Using default WiFi credentials");
      Serial.println("SSID: " + String(ssid));
    }

    // Clean disconnect first
    WiFi.disconnect(true);
    delay(1000);

    // Set WiFi mode
    WiFi.mode(WIFI_STA);

    Serial.println("Connecting to WiFi...");
    Serial.print("Attempting to connect to: ");
    Serial.println(ssid);

    // Start connection
    WiFi.begin(ssid.c_str(), password.c_str());

    // Wait for connection with timeout
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
        Serial.print(".");
        delay(500);
        attempts++;
        yield(); // Prevent watchdog reset

        // Print status every 5 attempts
        if (attempts % 10 == 0) {
            Serial.print(" Status: ");
            Serial.println(WiFi.status());
        }
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("");
        Serial.println("WiFi connected successfully!");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        Serial.print("Signal strength: ");
        Serial.print(WiFi.RSSI());
        Serial.println(" dBm");
        Serial.print("Gateway: ");
        Serial.println(WiFi.gatewayIP());
        digitalWrite(led, LOW); // Turn off LED

        // Enable auto-reconnect after successful connection
        WiFi.setAutoReconnect(true);
    } else {
        Serial.println("");
        Serial.println("Failed to connect to WiFi!");
        Serial.print("Final WiFi status: ");
        Serial.println(WiFi.status());
        Serial.println("Status codes: 0=IDLE, 1=NO_SSID, 3=CONNECTED, 4=CONNECT_FAILED, 6=DISCONNECTED");
        digitalWrite(led, HIGH); // Keep LED on to indicate failure

        // Try to start AP mode as fallback
        Serial.println("Starting AP mode as fallback...");
        WiFi.mode(WIFI_AP);
        WiFi.softAP("NodeMCU-Setup", "12345678");
        Serial.print("AP IP address: ");
        Serial.println(WiFi.softAPIP());
    }
}
