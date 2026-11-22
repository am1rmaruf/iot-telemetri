
import paho.mqtt.client as mqtt
import json
import requests

BROKER = "localhost"
PORT = 1883
TOPIC = "sensors/#"

RECEIVER_URL = "http://localhost:5000/telemetry"


def on_connect(client, userdata, flags, reason_code, properties):
    print("Ansluten till MQTT:", reason_code)
    client.subscribe(TOPIC)


def on_message(client, userdata, msg):
    try:
        data = json.loads(msg.payload.decode())
        print("Mottagen telemetri:", data)

        response = requests.post(RECEIVER_URL, json=data, timeout=5)
        print("Skickat vidare:", response.status_code)

    except Exception as error:
        print("Fel:", error)


client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.on_connect = on_connect
client.on_message = on_message

client.connect(BROKER, PORT)
client.loop_forever()
