#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME ""
#define BLYNK_AUTH_TOKEN ""

#include <Wire.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Adafruit_ADXL345_U.h>

// WiFi credentials
char ssid[] = "";
char pass[] = "";

// Receiver ESP32 IP and port
const char* receiverIP = "";
const int receiverPort = 1234;
WiFiClient client;

// Sensor pins and thresholds
#define FLEX_SENSOR_PIN 34
#define FLEX_BEND_THRESHOLD 2500
#define FLEX_HYSTERESIS 200

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
BlynkTimer timer;
bool flexAlertSent = false;

void setup() {
  Serial.begin(115200);

  // WiFi & Blynk setup
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Initialize accelerometer
  if (!accel.begin()) {
    Serial.println("ADXL345 not detected");
    while (1);
  }

  pinMode(FLEX_SENSOR_PIN, INPUT);
  timer.setInterval(1000L, checkFlexSensor); // Check flex sensor every second
}

void loop() {
  Blynk.run();
  timer.run();

  // Gesture detection
  sensors_event_t event;
  accel.getEvent(&event);

  String command = "STOP";
  if (event.acceleration.x > 2) command = "RIGHT";
  else if (event.acceleration.x < -2) command = "LEFT";
  else if (event.acceleration.y > 2) command = "FORWARD";

  // Send command to receiver
  if (client.connect(receiverIP, receiverPort)) {
    client.print(command + "\n");
    client.stop();
  }

  // Blynk event logging
  if (command != "STOP") {
    Serial.println("Gesture Detected: " + command);
    Blynk.logEvent("gesture_detected", "Gesture: " + command);
  }

  delay(100); // Short delay between reads
}

// Flex sensor emergency check
void checkFlexSensor() {
  int flexValue = analogRead(FLEX_SENSOR_PIN);
  Serial.print("Flex Sensor Value: ");
  Serial.println(flexValue);

  Blynk.virtualWrite(V1, flexValue);

  if (flexValue > FLEX_BEND_THRESHOLD && !flexAlertSent) {
    Serial.println(" Emergency: Flex sensor bending detected!");
    Blynk.logEvent("emergency", "Emergency Alert Triggered!");
    Blynk.virtualWrite(V2, 255);  // LED widget ON
    flexAlertSent = true;
  } else if (flexValue < (FLEX_BEND_THRESHOLD - FLEX_HYSTERESIS)) {
    flexAlertSent = false;
    Blynk.virtualWrite(V2, 0);  // LED widget OFF
  }
}
