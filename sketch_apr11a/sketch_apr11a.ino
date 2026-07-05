#include <SoftwareSerial.h>
#include <HUSKYLENS.h>
#include <Wire.h>

// Motor Pins
const int L_IN1 = 22, L_IN2 = 23, L_ENA = 5;
const int R_IN3 = 24, R_IN4 = 25, R_ENB = 6;

// --- Water Sensors and Alert --- (Keep this for other sensor functionality)
const int relayPin = 7, buzzerPin = 8;
unsigned long relayTimer = 0;
const unsigned long relayDuration = 3000;

// Bluetooth setup
SoftwareSerial BT(10, 11);
// HuskyLens setup
HUSKYLENS huskylens;

// Motor speed
const int maxSpeed = 190;

// Tracking and proximity settings
const int objWidth = 50; // The object width threshold
bool objectDetectedClose = false;

void setup() {
  Serial.begin(115200);   // Serial monitor for debugging
  BT.begin(9600);        // Bluetooth communication

  // Motor pins setup
  pinMode(L_IN1, OUTPUT); pinMode(L_IN2, OUTPUT); pinMode(L_ENA, OUTPUT);
  pinMode(R_IN3, OUTPUT); pinMode(R_IN4, OUTPUT); pinMode(R_ENB, OUTPUT);
  pinMode(relayPin, OUTPUT); pinMode(buzzerPin, OUTPUT);

  // Initialize motors to stop
  stopMove();

  // HuskyLens setup via I2C
  Wire.begin();
  while (!huskylens.begin(Wire)) {
    Serial.println("HuskyLens I2C fail; check connections.");
    delay(200);
  }
  huskylens.writeAlgorithm(ALGORITHM_OBJECT_TRACKING);
  Serial.println("HuskyLens ready!");
}

void loop() {
  if (BT.available()) {
    char c = BT.read();
    Serial.print("BT Received: "); Serial.println(c); // Show received command
    handleBT(c);
  }

  if (huskylens.request() && huskylens.available()) {
    HUSKYLENSResult result = huskylens.read();
    Serial.print("Tracking: xCenter="); Serial.print(result.xCenter);
    Serial.print(" | width="); Serial.println(result.width);

    // Check for object proximity for the alert
    if (result.width > objWidth + 20) {
      digitalWrite(relayPin, HIGH);   // Turn on relay
      digitalWrite(buzzerPin, HIGH);  // Turn on buzzer
      relayTimer = millis();         // Start the relay timer
      objectDetectedClose = true;
    } else {
      digitalWrite(relayPin, LOW);    // Turn off relay
      digitalWrite(buzzerPin, LOW);    // Turn off buzzer
      objectDetectedClose = false;
    }

    // HuskyLens tracking information is still available for other purposes
    // You can add code here to log or use the tracking data if needed.

  } else {
    // No object detected by HuskyLens
    objectDetectedClose = false;
  }

  // Relay auto-off after the specified duration
  if (relayTimer != 0 && millis() - relayTimer > relayDuration) {
    digitalWrite(relayPin, LOW);
    digitalWrite(buzzerPin, LOW);
    relayTimer = 0;
  }
}

// Function to handle Bluetooth commands
void handleBT(char c) {
  switch (c) {
    case 'F': moveForward();   Serial.println("Command: Forward"); break;   // Move forward
    case 'B': moveBackward();  Serial.println("Command: Backward"); break;  // Move backward
    case 'L': turnLeft();      Serial.println("Command: Left"); break;      // Turn left
    case 'R': turnRight();     Serial.println("Command: Right"); break;     // Turn right
    case 'S': stopMove();      Serial.println("Command: Stop"); break;      // Stop motors
    default:
      Serial.println("Invalid Command");
      break;
  }
}

// Function to control motor speed
void setMotorSpeed(int lSpeed, int rSpeed) {
  analogWrite(L_ENA, lSpeed);
  analogWrite(R_ENB, rSpeed);
}

// Functions to move the robot
void moveForward() {
  digitalWrite(L_IN1, HIGH); digitalWrite(L_IN2, LOW);
  digitalWrite(R_IN3, HIGH); digitalWrite(R_IN4, LOW);
  analogWrite(L_ENA, maxSpeed);
  analogWrite(R_ENB, maxSpeed);
}

void moveBackward() {
  digitalWrite(L_IN1, LOW); digitalWrite(L_IN2, HIGH);
  digitalWrite(R_IN3, LOW); digitalWrite(R_IN4, HIGH);
  analogWrite(L_ENA, maxSpeed);
  analogWrite(R_ENB, maxSpeed);
}

void turnLeft() {
  digitalWrite(L_IN1, LOW); digitalWrite(L_IN2, HIGH);
  digitalWrite(R_IN3, HIGH); digitalWrite(R_IN4, LOW);
  analogWrite(L_ENA, maxSpeed);
  analogWrite(R_ENB, maxSpeed);
}

void turnRight() {
  digitalWrite(L_IN1, HIGH); digitalWrite(L_IN2, LOW);
  digitalWrite(R_IN3, LOW); digitalWrite(R_IN4, HIGH);
  analogWrite(L_ENA, maxSpeed);
  analogWrite(R_ENB, maxSpeed);
}

void stopMove() {
  digitalWrite(L_IN1, LOW); digitalWrite(L_IN2, LOW);
  digitalWrite(R_IN3, LOW); digitalWrite(R_IN4, LOW);
  analogWrite(L_ENA, 0);
  analogWrite(R_ENB, 0);
}