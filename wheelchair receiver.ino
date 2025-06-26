#include <WiFi.h>

// WiFi credentials (same as transmitter)
const char* ssid = "";
const char* password = "";

// Server to receive command from transmitter
WiFiServer server(1234);

// Motor control pins
#define MOTOR_LEFT_1  14
#define MOTOR_LEFT_2  27
#define MOTOR_RIGHT_1 26
#define MOTOR_RIGHT_2 25

// IR sensor and buzzer pins
#define IR_SENSOR_LEFT  4
#define IR_SENSOR_RIGHT 5
#define BUZZER_PIN      19

void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  server.begin();
  Serial.println("Server started, waiting for commands...");

  // Motor pin modes
  pinMode(MOTOR_LEFT_1, OUTPUT);
  pinMode(MOTOR_LEFT_2, OUTPUT);
  pinMode(MOTOR_RIGHT_1, OUTPUT);
  pinMode(MOTOR_RIGHT_2, OUTPUT);
  stopMotors();

  // Sensor & buzzer setup
  pinMode(IR_SENSOR_LEFT, INPUT);
  pinMode(IR_SENSOR_RIGHT, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("Client connected...");
    String command = client.readStringUntil('\n');
    command.trim();
    Serial.println("Received Command: " + command);

    // Obstacle check
    if (detectObstacle()) {
      Serial.println("Obstacle detected. Stopping...");
      stopMotors();
      activateBuzzer();
    } else {
      if (command == "FORWARD") moveForward();
      else if (command == "LEFT") turnLeft();
      else if (command == "RIGHT") turnRight();
      else if (command == "STOP") stopMotors();
    }

    client.stop(); // Close connection
  }
}

// Motor control functions
void moveForward() {
  Serial.println("Moving Forward");
  digitalWrite(MOTOR_LEFT_1, HIGH);
  digitalWrite(MOTOR_LEFT_2, LOW);
  digitalWrite(MOTOR_RIGHT_1, HIGH);
  digitalWrite(MOTOR_RIGHT_2, LOW);
}

void turnLeft() {
  Serial.println("Turning Left");
  digitalWrite(MOTOR_LEFT_1, LOW);
  digitalWrite(MOTOR_LEFT_2, HIGH);
  digitalWrite(MOTOR_RIGHT_1, HIGH);
  digitalWrite(MOTOR_RIGHT_2, LOW);
}

void turnRight() {
  Serial.println("Turning Right");
  digitalWrite(MOTOR_LEFT_1, HIGH);
  digitalWrite(MOTOR_LEFT_2, LOW);
  digitalWrite(MOTOR_RIGHT_1, LOW);
  digitalWrite(MOTOR_RIGHT_2, HIGH);
}

void stopMotors() {
  Serial.println("Stopping Motors");
  digitalWrite(MOTOR_LEFT_1, LOW);
  digitalWrite(MOTOR_LEFT_2, LOW);
  digitalWrite(MOTOR_RIGHT_1, LOW);
  digitalWrite(MOTOR_RIGHT_2, LOW);
}

// Obstacle detection
bool detectObstacle() {
  bool left = digitalRead(IR_SENSOR_LEFT) == LOW;
  bool right = digitalRead(IR_SENSOR_RIGHT) == LOW;
  return left || right;
}

// Buzzer on obstacle
void activateBuzzer() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(1000);
  digitalWrite(BUZZER_PIN, LOW);
}
