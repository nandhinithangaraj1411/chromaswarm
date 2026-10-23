from flask import Flask, request, jsonify

print("🔥 THIS SERVER FILE IS RUNNING 🔥")

app = Flask(__name__)
data_store = {}

# -------- RECEIVE DATA FROM NODES --------
@app.route('/update', methods=['POST'])
def update():
    try:
        data = request.json
        print("Received:", data)

        node = str(data["node"])   # ✅ correct key

        data_store[node] = data

        return jsonify({"msg": "received"})
    
    except Exception as e:
        print("ERROR:", e)
        return jsonify({"error": str(e)}), 500


# -------- SEND FULL DATA TO DASHBOARD --------
@app.route('/data', methods=['GET'])
def get_data():
    return jsonify(data_store)


# -------- SWARM: SHARE ONLY ALERT STATUS --------
@app.route('/swarm', methods=['GET'])
def swarm():
    swarm_status = {}

    for node, info in data_store.items():
        swarm_status[node] = {
            "status": info["status"]   # ✅ ONLY sharing alert info
        }

    return jsonify(swarm_status)
control_command = {
    "mode": "NORMAL",
    "target": "ALL"   # can be "ALL", "1", "2"
}

@app.route('/control', methods=['POST'])
def control():
    global control_command
    control_command = request.json
    print("CONTROL:", control_command)
    return jsonify({"msg": "command received"})

@app.route('/get_control', methods=['GET'])
def get_control():
    return jsonify(control_command)


# -------- RUN SERVER --------
app.run(host='0.0.0.0', port=8000)