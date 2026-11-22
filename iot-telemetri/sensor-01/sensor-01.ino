
#include <WiFi.h>
#include <PubSubClient.h>

// WiFi-inställningar
// Fyll i dina egna uppgifter
const char* ssid = "DITT_WIFI_NAMN";
const char* password = "DITT_WIFI_LOSENORD";

// Raspberry Pi och MQTT
const char* mqttServer = "192.168.0.120";
const int mqttPort = 1883;

// ESP32 nr 1
const char* deviceId = "sensor-01";
const char* mqttTopic = "sensors/sensor-01";

// HC-SR04, samma pinnar som i din gamla uppgift
const int trigPin = 27;
const int echoPin = 26;

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

// Anslut till WiFi
void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }

  Serial.print("Ansluter till WiFi");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi anslutet!");
  Serial.print("IP-adress: ");
  Serial.println(WiFi.localIP());
}

// Anslut till MQTT
void connectMQTT() {
  while (!mqtt.connected()) {
    Serial.println("Ansluter till MQTT...");

    if (mqtt.connect(deviceId)) {
      Serial.println("MQTT ansluten!");
    } else {
      Serial.print("MQTT felkod: ");
      Serial.println(mqtt.state());
      delay(3000);
    }
  }
}

// Gör en avståndsmätning
float measureDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  unsigned long duration = pulseIn(echoPin, HIGH, 30000);

  if (duration == 0) {
    return -1;
  }

  float distance = duration * 0.0343 / 2.0;

  // Ignorera ogiltiga mätvärden
  if (distance < 2 || distance > 400) {
    return -1;
  }

  return distance;
}

// Gör fem mätningar för stabilare resultat
float measureStableDistance() {
  float total = 0;
  int validMeasurements = 0;

  for (int i = 0; i < 5; i++) {
    float distance = measureDistance();

    if (distance >= 2 && distance <= 400) {
      total += distance;
      validMeasurements++;
    }

    delay(60);
  }

  if (validMeasurements == 0) {
    return -1;
  }

  return total / validMeasurements;
}

// Skicka sensordata till Raspberry Pi
void sendMeasurement(float distance) {
  String json = "{\"deviceId\":\"" + String(deviceId) +
                "\",\"value\":" + String(distance, 1) +
                ",\"unit\":\"cm\"}";

  bool sent = mqtt.publish(mqttTopic, json.c_str());

  Serial.println(json);

  if (sent) {
    Serial.println("Skickat till MQTT!");
  } else {
    Serial.println("Kunde inte skicka till MQTT");
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  digitalWrite(trigPin, LOW);

  connectWiFi();

  mqtt.setServer(mqttServer, mqttPort);
}

void loop() {
  // Kontrollera WiFi
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  // Kontrollera MQTT
  if (!mqtt.connected()) {
    connectMQTT();
  }

  mqtt.loop();

  // Läs av sensorn
  float distance = measureStableDistance();

  if (distance < 0) {
    Serial.println("Ingen giltig sensormätning, skickar inte");
  } else {
    Serial.print("Avstand: ");
    Serial.print(distance);
    Serial.println(" cm");

    sendMeasurement(distance);
  }

  delay(5000);
}
