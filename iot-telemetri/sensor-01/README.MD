
# IoT Telemetri med ESP32 och Raspberry Pi

## Om uppgiften

I den här uppgiften har jag byggt ett IoT-system där två ESP32-enheter skickar mätvärden via MQTT till en Raspberry Pi.

Tanken från början var att använda Azure IoT Hub, men jag fick problem med mitt Azure-konto och kunde inte skapa någon IoT Hub. Jag valde därför att göra en lokal lösning med Python och Flask istället.

Jag har framför allt fokuserat på att få kommunikationen mellan enheterna att fungera.

## Utrustning

Jag har använt:
- Två ESP32
- En Raspberry Pi
- En HC-SR04 ultraljudssensor
- Breadboard och kopplingskablar
- Arduino IDE och VS Code

## ESP32-enheterna

### ESP32 nr 1

Den första ESP32-enheten använder en HC-SR04 för att mäta avstånd i centimeter.

Jag hade lite problem med sensorn i början eftersom den inte gav några giltiga mätvärden. Efter att jag kontrollerat kopplingarna och ändrat GPIO-inställningarna började den fungera.

Jag lade också in en kontroll i koden så att ogiltiga mätningar inte skickas vidare.

Den skickar till MQTT-topic:

`sensors/sensor-01`

### ESP32 nr 2

Eftersom jag bara hade en extern sensor använde jag temperaturfunktionen som finns i den andra ESP32-enheten.

Den läser av chiptemperaturen och skickar värdet via MQTT. Temperaturen är inte kalibrerad och ska inte jämföras med vanlig rumstemperatur.

Den använder:

`sensors/sensor-02`

## Raspberry Pi och MQTT

Jag använder Raspberry Pi som MQTT-broker med Mosquitto.

Båda ESP32-enheterna ansluter till samma broker via WiFi och skickar sina mätvärden som JSON.

För att kontrollera att båda skickar data använde jag:

    mosquitto_sub -h localhost -t "sensors/#" -v

I terminalen kunde jag se meddelanden från båda ESP32-enheterna samtidigt.

## Python-gateway

Jag har skrivit en gateway i Python som prenumererar på MQTT-meddelandena.

När gatewayen får ett meddelande skickar den vidare informationen till Flask med en HTTP POST-förfrågan.

Jag testade detta och fick statuskod 201, vilket visade att mottagaren hade tagit emot mätvärdet.

## Flask istället för Azure

Eftersom jag inte kunde använda Azure byggde jag en lokal mottagare med Flask.

Den tar emot mätvärden från gatewayen och sparar dem tillfälligt i minnet.

Jag kunde sedan kontrollera mottagna värden genom att skriva:

    curl http://127.0.0.1:5000/telemetry

Där såg jag mätvärden från både sensor-01 och sensor-02.

## Så startar jag systemet

Jag börjar med att kontrollera att Mosquitto körs på Raspberry Pi:

    sudo systemctl status mosquitto

Sedan startar jag Flask:

    source iot-env/bin/activate
    python local_receiver/receiver.py

I en annan SSH-terminal startar jag gatewayen:

    source iot-env/bin/activate
    python gateway/gateway.py

När båda ESP32-enheterna är anslutna till ström börjar de skicka sina mätvärden.

## Test och resultat

Jag har testat systemet steg för steg.

Först kontrollerade jag att ESP32-enheterna kunde ansluta till WiFi och MQTT. Sedan testade jag att Raspberry Pi tog emot meddelanden från båda enheterna.

Jag hade en del problem med avståndssensorn eftersom vissa mätningar blev fel eller inte registrerades alls. Efter felsökning och ändringar i koden fick jag den att fungera bättre.

Till sist testade jag Python-gatewayen och Flask. Jag kunde se att meddelandena skickades vidare och att Flask tog emot data från båda enheterna.

## Det som inte blev som planerat

Den största skillnaden jämfört med uppgiften är att jag inte använde Azure IoT Hub.

Jag försökte få igång Azure via mitt skolkonto, men när jag skulle skapa en IoT Hub fanns det ingen tillgänglig prenumeration att välja.

Därför byggde jag en lokal lösning med Flask istället. Jag är medveten om att det inte är samma sak som en riktig Azure-koppling, men jag ville ändå få själva dataflödet att fungera.

Jag använde också ESP32:s interna chiptemperatur för den andra enheten eftersom jag bara hade en extern sensor.

## Sammanfattning

Jag fick till slut båda ESP32-enheterna att skicka mätvärden till Raspberry Pi via MQTT.

Med hjälp av en Python-gateway kunde jag sedan skicka vidare informationen till Flask och läsa av värdena där.

Det jag framför allt har lärt mig är hur MQTT fungerar i praktiken, hur olika enheter kommunicerar över ett nätverk och hur man kan skicka vidare sensordata mellan olika program.
