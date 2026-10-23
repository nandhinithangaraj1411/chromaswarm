#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <HTTPClient.h>

// -------- WIFI --------
const char* ssid = "Nandhini's Phone";
const char* password = "vszv2377";

const char* serverURL = "http://10.64.149.101:8000/update";

int node_id = 1;

// -------- TIMING --------
unsigned long lastSendTime = 0;
unsigned long sendInterval = 1500;

// -------- PINS --------
#define PIR 13
#define LDR 34
#define LEDPIN 5

#define TRIG 18
#define ECHO 35

// -------- COLOR SENSOR --------
#define S0 27
#define S1 26
#define S2 25
#define S3 33
#define OUT 32

// -------- NEOPIXEL --------
#define NUMPIXELS 12
Adafruit_NeoPixel strip(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);

// -------- MOTION HOLD --------
unsigned long lastMotionTime = 0;
unsigned long alertDuration = 4000;

// -------- STABILITY --------
String currentMode = "";
unsigned long lastSwarmTime = 0;

String proximityStatus = "";
float distance = 0;

void readUltrasonic();
void setColor(int r, int g, int b);
void setup() {
  Serial.begin(115200);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
Serial.println("BOOTING...");
Serial.println("Connecting to WiFi...");

WiFi.begin(ssid, password);

while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
}

Serial.println();
Serial.println("WiFi Connected!");
Serial.print("ESP IP: ");
Serial.println(WiFi.localIP());
  pinMode(PIR, INPUT);

  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(OUT, INPUT);

  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  strip.begin();
  strip.setBrightness(20);
  strip.show();
}

void loop() {

  // -------- SENSOR READ --------
  int motion = digitalRead(PIR);
  int light = analogRead(LDR);
  Serial.println(digitalRead(PIR));
  readUltrasonic();
  String lightStatus = (light < 1000) ? "DARK" :
                       (light < 3000) ? "MEDIUM" : "BRIGHT";
                       

  // -------- COLOR READ --------
  int red, green, blue;

  digitalWrite(S2, LOW); digitalWrite(S3, LOW);
  red = pulseIn(OUT, LOW);

  digitalWrite(S2, HIGH); digitalWrite(S3, HIGH);
  green = pulseIn(OUT, LOW);

  digitalWrite(S2, LOW); digitalWrite(S3, HIGH);
  blue = pulseIn(OUT, LOW);

  String detectedColor;

  if (red < 65 && green > 90 && blue > 70) detectedColor = "RED";
  else if (red < 70 && green < 80 && blue > 75) detectedColor = "GREEN";
  else if (blue < red && blue < green && blue < 105) detectedColor = "BLUE";
  else detectedColor = "UNKNOWN";

  // -------- MOTION MEMORY --------
  static bool motionActive = false;

  if (motion == HIGH) {
    motionActive = true;
    lastMotionTime = millis();
  }

  String motionStatus = motionActive ? "DETECTED" : "NONE";
  String finalStatus = "";

  int r = 0, g = 0, b = 0;

  // -------- LOCAL LOGIC --------
  if (motionActive && millis() - lastMotionTime < alertDuration) {
    r = 255; g = 0; b = 255;
    finalStatus = "ALERT (MOTION)";
  } else {
    motionActive = false;

    if (detectedColor == "RED") {
        setColor(255, 0, 0);
    }
    else if (detectedColor == "GREEN") {
        setColor(0, 255, 0);
    }
    else if (detectedColor == "BLUE") {
        setColor(0, 0, 255);
    }
    else {
        setColor(30, 30, 30);
    }

    finalStatus = "CAMOUFLAGE MODE";

    delay(50);  // 🔥 small stabilization delay
}

  // -------- WIFI --------
  if (millis() - lastSendTime > sendInterval && WiFi.status() == WL_CONNECTED) {

    // -------- SEND DATA --------
    HTTPClient http;
    http.begin(serverURL);
    http.addHeader("Content-Type", "application/json");

    String displayColor;

// Decide what color is ACTUALLY being shown
if (finalStatus.indexOf("ALERT") != -1) {
    displayColor = "ALERT";
} else {
    displayColor = detectedColor;
}

String json = "{";
json += "\"node\":" + String(node_id) + ",";
json += "\"color\":\"" + displayColor + "\",";   // 🔥 changed here
json += "\"motion\":\"" + motionStatus + "\",";
json += "\"light\":\"" + lightStatus + "\",";
json += "\"distance\":" + String(distance) + ",";
json += "\"proximity\":\"" + proximityStatus + "\",";
json += "\"status\":\"" + finalStatus + "\"";
json += "}";

    int httpResponseCode = http.POST(json);

Serial.print("POST Response: ");
Serial.println(httpResponseCode);
    http.end();

    delay(200);

    // -------- SWARM --------
    HTTPClient http2;
    http2.begin("http://10.64.149.101:8000/swarm");

    if (http2.GET() == 200) {
      String payload = http2.getString();

      if (payload.indexOf("ALERT (") != -1 && finalStatus.indexOf("ALERT") == -1) {
    lastSwarmTime = millis();
}
      }
    
    http2.end();

    // Apply swarm HOLD (3 sec stability)
    if (millis() - lastSwarmTime < 3000) {
      r = 255; g = 0; b = 255;
      finalStatus = "ALERT (SWARM)";
    }

    // -------- CONTROL --------
    HTTPClient http3;
    http3.begin("http://10.64.149.101:8000/get_control");

    if (http3.GET() == 200) {
      String payload = http3.getString();

      if (payload.indexOf("\"target\":\"ALL\"") != -1 ||
          payload.indexOf("\"target\":\"" + String(node_id) + "\"") != -1) {

        if (payload.indexOf("FORCE_ALERT") != -1) {
          r = 255; g = 0; b = 0;
          finalStatus = "FORCED ALERT";
        }
        else if (payload.indexOf("OFF") != -1) {
          r = 0; g = 0; b = 0;
          finalStatus = "OFF";
        }
      }
    }
    http3.end();

    lastSendTime = millis();
  }

  // -------- APPLY ONLY IF CHANGED --------
  if (finalStatus != currentMode) {
    currentMode = finalStatus;
    setColor(r, g, b);
  }

  delay(200);
}

void readUltrasonic() {

  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH, 30000);

  distance = duration * 0.034 / 2;

  if(duration == 0){
    proximityStatus = "OUT OF RANGE";
  }
  else if(distance < 10){
    proximityStatus = "CRITICAL ALERT";
  }
  else if(distance < 20){
    proximityStatus = "HIGH ALERT";
  }
  else if(distance < 50){
    proximityStatus = "CAUTION";
  }
  else if(distance < 100){
    proximityStatus = "OBJECT DETECTED";
  }
  else{
    proximityStatus = "SAFE";
  }

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm | Status: ");
  Serial.println(proximityStatus);
}

// -------- LED FUNCTION --------
void setColor(int r, int g, int b) {
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, strip.Color(r, g, b));
  }
  strip.show();
}