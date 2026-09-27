// Data sending in JSON format

#include <WiFi.h>
#include <PubSubClient.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// ================= USER CONFIG =================
const char* ssid = "Switchsys";
const char* password = "11223344";

const char* mqtt_server = "mqtt.switchsys.in";
const int mqtt_port = 1883;

const char* chip_temp_topic = "switchsysmqtt/esp32c3/chip_temp";

WiFiClient espClient;
PubSubClient client(espClient);

// Non-blocking timer variables
unsigned long lastPublishTime = 0;
const unsigned long PUBLISH_INTERVAL_MS = 1000; // Publish every 1 second (1000ms)

unsigned long lastReconnectAttempt = 0;

// ================= WIFI =================
void setup_wifi() {
  Serial.println("Connecting WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

// ================= NON-BLOCKING MQTT RECONNECT =================
bool reconnect_mqtt() {
  if (client.connect("ESP32C3_Super_Mini")) {
    Serial.println("MQTT Connected");
    return true;
  } else {
    Serial.print("MQTT Connect failed, rc=");
    Serial.println(client.state());
    return false;
  }
}

// ================= FAST TEMP SENSING =================
float getChipTempFast() {
  // Direct reading without artificial delay calls
  return temperatureRead();
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);
  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);
  client.setBufferSize(256);
}

// ================= LOOP =================
void loop() 
{
  // 1. Maintain WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    setup_wifi();
  }

  // 2. Non-blocking MQTT reconnect handler
  if (!client.connected()) {
    unsigned long now = millis();
    if (now - lastReconnectAttempt > 3000) { // Retry every 3 seconds without freezing
      lastReconnectAttempt = now;
      if (reconnect_mqtt()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {
    // 3. Process MQTT client loop continuously
    client.loop();
  }

  // 4. Send telemetry using non-blocking millis() timer
  unsigned long currentMillis = millis();
    if (currentMillis - lastPublishTime >= PUBLISH_INTERVAL_MS) {
    lastPublishTime = currentMillis;

    if (client.connected()) {
      float chipTempfloat = getChipTempFast();
      char tempStr[16];
      dtostrf(chipTempfloat, 0, 1, tempStr); // 1 decimal place (e.g. "42.5")

      char jsonBuffer[48];
      snprintf(jsonBuffer, sizeof(jsonBuffer), "{\"chip_temp\":%s}", tempStr);

      client.publish(chip_temp_topic, jsonBuffer);
      Serial.print("Published Chip Temp: ");
      Serial.println(jsonBuffer);
      delay(10000);
    }
  }
}