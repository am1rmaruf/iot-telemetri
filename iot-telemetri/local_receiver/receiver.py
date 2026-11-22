
from flask import Flask, request, jsonify

app = Flask(__name__)

telemetry = []


@app.route("/telemetry", methods=["POST"])
def receive_telemetry():
    data = request.get_json()
    telemetry.append(data)

    print("Mottagen telemetri:", data)

    return jsonify({"status": "received"}), 201


@app.route("/telemetry", methods=["GET"])
def get_telemetry():
    return jsonify(telemetry)


if __name__ == "__main__":
    app.run(port=5000)
