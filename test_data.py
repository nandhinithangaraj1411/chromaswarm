import requests
import random
import time

url = "http://127.0.0.1:8000/update"

while True:
    data = {
        "node_id": random.randint(1,3),
        "status": random.choice(["ALERT", "STEALTH"]),
        "motion": random.randint(0,1),
        "ir": random.randint(0,1),
        "ldr": random.randint(100,800),
        "color": random.choice(["RED", "GREEN", "BLUE"])
    }

    requests.post(url, json=data)
    print("Sent:", data)

    time.sleep(2)