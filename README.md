# ChromaSwarm — Bio-Inspired Distributed Monitoring System
A sensor-integrated swarm monitoring system combining embedded hardware, real-time data visualization, and coordinated node control.

## Overview

ChromaSwarm is a bio-inspired distributed monitoring system designed to detect environmental changes and potential intrusions using multiple sensor-equipped nodes. The system integrates Arduino-based hardware with a Python backend and a real-time dashboard to monitor node activity, visualize sensor readings, and coordinate responses through control commands.

The project explores distributed sensing, inter-node status sharing, and hardware–software integration for responsive environmental monitoring.

## Key Features

- **Multi-Node Monitoring:** Collects and displays status information from multiple sensor nodes.
- **Real-Time Dashboard:** Visualizes node status, environmental readings, and active alerts through an interactive interface.
- **Sensor Data Visualization:** Displays light and motion activity trends over time.
- **Swarm Status Sharing:** Provides an API endpoint for sharing node alert statuses.
- **Remote Node Control:** Supports commands to trigger alerts, restore normal operation, and turn off LEDs.
- **Embedded Hardware Integration:** Connects sensor-based Arduino firmware with the software monitoring pipeline.
- **Simulation Support:** Includes a Python script for generating test sensor data.

## System Architecture

The system consists of three primary software and hardware components:

| Component            | Technology                | Responsibility                                                           |
| -------------------- | ------------------------- | ------------------------------------------------------------------------ |
| Embedded Nodes       | Arduino, C/C++            | Sensor interfacing and hardware control                                  |
| Backend Server       | Python, Flask, REST APIs  | Receives sensor data, maintains node state, and handles control commands |
| Monitoring Dashboard | Python, Streamlit, Pandas | Displays node status, alerts, sensor readings, and activity graphs       |
| Test Data Generator  | Python, Requests          | Sends simulated sensor data for testing                                  |

## Technology Stack

**Languages:** C/C++, Python
**Embedded Systems:** Arduino
**Backend:** Flask, REST APIs
**Frontend / Visualization:** Streamlit
**Data Processing:** Pandas
**HTTP Communication:** Requests

## System Workflow

1. Sensor nodes collect environmental and motion-related information.
2. Nodes transmit their readings to the Flask backend through HTTP requests.
3. The backend stores the latest data received from each node.
4. The Streamlit dashboard retrieves the stored data and displays node statuses, alerts, and sensor activity graphs.
5. Control commands submitted through the dashboard are sent to the backend for retrieval by connected nodes.
6. The backend exposes a swarm-status endpoint for sharing node alert information.

## Project Structure

```text
chromaswarm/
├── elsem2/
│   └── elsem2.ino
├── app.py
├── server.py
├── test_data.py
├── requirements.txt
└── README.md
```

- `elsem2/elsem2.ino` — Arduino firmware.
- `server.py` — Flask server, data ingestion, status retrieval, and control endpoints.
- `app.py` — Streamlit monitoring dashboard and sensor activity visualization.
- `test_data.py` — Script for generating simulated sensor readings.
- `requirements.txt` — Python dependency list.

## Getting Started

### Prerequisites

- Python 3.10 or a compatible Python version
- Arduino IDE
- Arduino-compatible hardware and required sensors
- Git

### 1. Clone the Repository

```bash
git clone https://github.com/nandhinithangaraj1411/chromaswarm.git
cd chromaswarm
```

### 2. Install Dependencies

```bash
pip install -r requirements.txt
```

The `requirements.txt` file should include:

```text
flask
streamlit
requests
pandas
```

### 3. Start the Backend

Open a terminal and run:

```bash
python server.py
```

The Flask server runs on port `8000` by default.

### 4. Launch the Dashboard

Open a second terminal and run:

```bash
streamlit run app.py
```

### 5. Configure the Hardware

- Open `elsem2/elsem2.ino` in the Arduino IDE.
- Select the appropriate board and port.
- Install the libraries required by the firmware.
- Configure the backend address in the Arduino and Python files to match your network setup.
- Upload the firmware to the connected board.

**Note:** The backend address must be reachable by the dashboard and hardware nodes. The current implementation uses a configured local network address; update it as required for your environment.

## API Endpoints

| Method | Endpoint       | Description                                                  |
| ------ | -------------- | ------------------------------------------------------------ |
| `POST` | `/update`      | Receives node data and updates the backend's in-memory state |
| `GET`  | `/data`        | Returns the latest stored data for all nodes                 |
| `GET`  | `/swarm`       | Returns node status information for swarm monitoring         |
| `POST` | `/control`     | Updates the current control command                          |
| `GET`  | `/get_control` | Returns the current control command                          |

## Current Scope and Future Improvements

Potential extensions include:

- Robust communication and coordination between distributed nodes.
- Improved fault tolerance and decentralized decision-making.
- Persistent storage and historical sensor analytics.
- Configurable node addressing and network settings.
- Enhanced security for node-to-server communication.
- Improved error handling and automated testing.
- Expanded digital-twin capabilities for synchronizing physical node states with their virtual representations.

## Contributors

Developed as a hardware–software integration project exploring embedded sensing, distributed monitoring, and swarm-inspired system design.


