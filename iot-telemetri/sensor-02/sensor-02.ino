
#include <WiFi.h>
#include <PubSubClient.h>
#include <math.h>

// Fyll i dina WiFi-uppgifter lokalt
const char* ssid = "DITT_WIFI_NAMN";
const char* password = "DITT_WIFI_LOSENORD";

// MQTT på Raspberry Pi
const char* mqttServer = "192.168.0.120";
const int mqttPort = 1883;

// Unikt ID för ESP32 nr 2
const char* deviceId = "sensor-02";
const char* mqttTopic = "sensors/sensor-02";

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Ansluter till WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi anslutet!");
  Serial.println(WiFi.localIP());
}

void connectMQTT() {
  while (!mqtt.connected()) {
    Serial.println("Ansluter till MQTT...");

    if (mqtt.connect(deviceId)) {
      Serial.println("MQTT ansluten!");
    } else {
      Serial.print("MQTT-fel: ");
      Serial.println(mqtt.state());
      delay(3000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  connectWiFi();
  mqtt.setServer(mqttServer, mqttPort);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  if (!mqtt.connected()) {
    connectMQTT();
  }

  mqtt.loop();

  float temperature = temperatureRead();

  if (!isnan(temperature) &&
      temperature > -40 &&
      temperature < 125) {

    String json = "{\"deviceId\":\"" + String(deviceId) +
                  "\",\"value\":" + String(temperature, 2) +
                  ",\"unit\":\"C\"}";

    bool sent = mqtt.publish(mqttTopic, json.c_str());

    Serial.println(json);
    Serial.println(sent ? "Skickat till MQTT!" :
                          "MQTT-publicering misslyckades");
  } else {
    Serial.println("Ingen giltig temperatur");
  }

  delay(5000);
}
