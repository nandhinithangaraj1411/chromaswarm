import streamlit as st
import requests
import time
import pandas as pd

st.set_page_config(page_title="ChromaSwarm", layout="wide")

# -------- HEADER --------
st.markdown("""
<div style="
    background: linear-gradient(90deg, #0f2027, #203a43, #2c5364);
    padding: 20px;
    border-radius: 15px;
    text-align: center;
    box-shadow: 0px 4px 20px rgba(0,0,0,0.5);
">
    <h1 style="color:white; margin-bottom:5px;">
        🐝 CHROMASWARM
    </h1>
    <h4 style="color:#00e6e6; margin-top:0;">
        Real-Time Swarm Intelligence Dashboard
    </h4>
</div>
""", unsafe_allow_html=True)

st.markdown("## 📡 Live Node Status")

placeholder = st.empty()

st.markdown("## 🎮 Control Panel")

col1, col2, col3 = st.columns(3)

with col1:
    if st.button("🚨 Trigger Swarm Alert"):
        requests.post("http://10.64.149.101:8000/control",
                      json={"mode": "FORCE_ALERT", "target": "ALL"})

with col2:
    if st.button("✅ Reset to Normal"):
        requests.post("http://10.64.149.101:8000/control",
                      json={"mode": "NORMAL", "target": "ALL"})

with col3:
    if st.button("🔴 Turn OFF LEDs"):
        requests.post("http://10.64.149.101:8000/control",
                      json={"mode": "OFF", "target": "ALL"})

# -------- HISTORY STORAGE --------
if "history" not in st.session_state:
    st.session_state.history = {}

# -------- COLOR BLOCK --------
def get_color_block(color):
    if color == "RED":
        return "<div style='background-color:red;height:40px;border-radius:10px'></div>"
    elif color == "GREEN":
        return "<div style='background-color:green;height:40px;border-radius:10px'></div>"
    elif color == "BLUE":
        return "<div style='background-color:blue;height:40px;border-radius:10px'></div>"
    else:
        return "<div style='background-color:gray;height:40px;border-radius:10px'></div>"

# -------- MAIN LOOP --------
while True:
    try:
        data = requests.get("http://10.64.149.101:8000/data").json()
        st.write(data)
        with placeholder.container():

            cols = st.columns(len(data) if data else 1)

            for i, (node, info) in enumerate(data.items()):
                with cols[i]:

                    # -------- INIT HISTORY --------
                    if node not in st.session_state.history:
                        st.session_state.history[node] = {
                            "light": [],
                            "motion": []
                        }

                    # Convert values
                    light_val = 1 if info["light"] == "BRIGHT" else 0
                    motion_val = 1 if info["motion"] == "DETECTED" else 0

                    # Store history
                    st.session_state.history[node]["light"].append(light_val)
                    st.session_state.history[node]["motion"].append(motion_val)

                    # Keep last 20 values
                    st.session_state.history[node]["light"] = st.session_state.history[node]["light"][-20:]
                    st.session_state.history[node]["motion"] = st.session_state.history[node]["motion"][-20:]

                    # -------- CARD --------
                    st.markdown("""
                    <div style="
                        padding:15px;
                        border-radius:15px;
                        background-color:#111;
                        box-shadow:0px 0px 10px rgba(0,0,0,0.5);
                    ">
                    """, unsafe_allow_html=True)

                    st.markdown(f"### 🔹 Node {node}")

                    # -------- ALERT --------
                    if "ALERT" in info["status"]:
                        st.markdown(
                            "<h4 style='color:red;'>🚨 ALERT ACTIVE</h4>",
                            unsafe_allow_html=True
                        )
                    else:
                        st.markdown(
                            "<h4 style='color:lightgreen;'>✅ NORMAL OPERATION</h4>",
                            unsafe_allow_html=True
                        )

                    # -------- INFO --------
                    st.markdown(f"""
                    **🧠 Current Mode:** {info['status']}  
                    **💡 Environment:** {info['light']} lighting  
                    **🕺 Motion Activity:** {info['motion']}  
                    **📏 Distance:** {info.get('distance', 'N/A')} cm  
                    **🚧 Proximity Status:** {info.get('proximity', 'N/A')} 
                    """)

                    # -------- COLOR --------
                    st.markdown("**🎨 Detected Color:**")
                    st.markdown(get_color_block(info["color"]), unsafe_allow_html=True)

                    # -------- GRAPHS --------
                    st.markdown("### 📊 Sensor Activity Over Time")

                    df = pd.DataFrame({
                        "Light": st.session_state.history[node]["light"],
                        "Motion": st.session_state.history[node]["motion"]
                    })

                    # LIGHT GRAPH
                    st.markdown("#### 💡 Light Intensity vs Time")
                    st.caption("X-axis: Time (seconds) | Y-axis: 0 = Dark, 1 = Bright")
                    st.line_chart(df["Light"])

                    # MOTION GRAPH
                    st.markdown("#### 🕺 Motion Detection vs Time")
                    st.caption("X-axis: Time (seconds) | Y-axis: 0 = No Motion, 1 = Motion Detected")
                    st.line_chart(df["Motion"])

                    st.markdown("</div>", unsafe_allow_html=True)

    except:
        st.warning("Waiting for nodes to send data...")

    time.sleep(1)