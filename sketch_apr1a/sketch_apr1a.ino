#include <SoftwareSerial.h>
#include <HUSKYLENS.h>
#include <Wire.h>
#include <math.h>

// Motor Pins
const int L_IN1 = 22, L_IN2 = 23, L_ENA = 5;
const int R_IN3 = 24, R_IN4 = 25, R_ENB = 6;

// --- Water Sensors and Alert ---
const int relayPin = 7, buzzerPin = 8;
unsigned long relayTimer = 0;
const unsigned long relayDuration = 3000;

// pH and TDS Sensor Pins
const int pHPin  = A0;
const int TDSPin = A1;

// Bluetooth setup
SoftwareSerial BT(10, 11); // RX, TX

// HuskyLens setup
HUSKYLENS huskylens;

// Motor speed
const int maxSpeed = 190;

// Tracking settings
const int objWidth = 50;
bool objectDetectedClose = false;

void setup() {
  Serial.begin(115200);
  BT.begin(9600);

  // Motor pins
  pinMode(L_IN1, OUTPUT); pinMode(L_IN2, OUTPUT); pinMode(L_ENA, OUTPUT);
  pinMode(R_IN3, OUTPUT); pinMode(R_IN4, OUTPUT); pinMode(R_ENB, OUTPUT);

  // Alert pins
  pinMode(relayPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  stopMove();

  // HuskyLens
  Wire.begin();
  while (!huskylens.begin(Wire)) {
    Serial.println("HuskyLens I2C fail; check connections.");
    delay(200);
  }
  huskylens.writeAlgorithm(ALGORITHM_OBJECT_TRACKING);
  Serial.println("HuskyLens ready!");
}

void loop() {
  // 1) Read sensors
  int rawPH  = analogRead(pHPin);
  int rawTDS = analogRead(TDSPin);

  float voltagePH  = rawPH  * (5.0 / 1023.0);
  float voltageTDS = rawTDS * (5.0 / 1023.0);

  float pHValue = 7.0 + (2.5 - voltagePH) / 0.18;  
  float tdsValue = (133.42 * pow(voltageTDS, 3)
                   - 255.86 * pow(voltageTDS, 2)
                   + 857.39 * voltageTDS) * 0.5;

  // 2) Print only pH and TDS
  Serial.print("pH: ");
  Serial.print(pHValue, 2);
  Serial.print(" | TDS: ");
  Serial.print(tdsValue, 1);
  Serial.println(" ppm");

  // 3) Automatic alert if thresholds crossed
  bool alert = false;
  if (pHValue > 8.5) alert = true;
  if (tdsValue > 300.0)               alert = true;

  if (alert && relayTimer == 0) {
    digitalWrite(relayPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
    relayTimer = millis();
  }

  // 4) Auto-off alert after duration
  if (relayTimer != 0 && millis() - relayTimer >= relayDuration) {
    digitalWrite(relayPin, LOW);
    digitalWrite(buzzerPin, LOW);
    relayTimer = 0;
  }

  // 5) Bluetooth control
  if (BT.available()) {
    char c = BT.read();
    Serial.print("BT Received: "); Serial.println(c);
    handleBT(c);
  }

  // 6) HuskyLens tracking (unchanged)
  if (huskylens.request() && huskylens.available()) {
    HUSKYLENSResult r = huskylens.read();
    Serial.print("Tracking: xCenter="); Serial.print(r.xCenter);
    Serial.print(" | width="); Serial.println(r.width);
    if (r.width > objWidth + 20) {
      digitalWrite(relayPin, HIGH);
      digitalWrite(buzzerPin, HIGH);
      relayTimer = relayTimer==0?millis():relayTimer;
      objectDetectedClose = true;
    } else {
      digitalWrite(relayPin, LOW);
      digitalWrite(buzzerPin, LOW);
      objectDetectedClose = false;
    }
  } else objectDetectedClose = false;
}

// Movement and helper functions (unchanged):
void handleBT(char c) {
  switch (c) {
    case 'F': moveForward();  Serial.println("Command: Forward");  break;
    case 'B': moveBackward(); Serial.println("Command: Backward"); break;
    case 'L': turnLeft();     Serial.println("Command: Left");     break;
    case 'R': turnRight();    Serial.println("Command: Right");    break;
    case 'S': stopMove();     Serial.println("Command: Stop");     break;
    default: Serial.println("Invalid Command"); break;
  }
}

void setMotorSpeed(int l, int r) {
  analogWrite(L_ENA, l);
  analogWrite(R_ENB, r);
}

void moveForward() {
  digitalWrite(L_IN1, HIGH); digitalWrite(L_IN2, LOW);
  digitalWrite(R_IN3, HIGH); digitalWrite(R_IN4, LOW);
  setMotorSpeed(maxSpeed, maxSpeed);
}
void moveBackward() {
  digitalWrite(L_IN1, LOW); digitalWrite(L_IN2, HIGH);
  digitalWrite(R_IN3, LOW); digitalWrite(R_IN4, HIGH);
  setMotorSpeed(maxSpeed, maxSpeed);
}
void turnLeft() {
  digitalWrite(L_IN1, LOW); digitalWrite(L_IN2, HIGH);
  digitalWrite(R_IN3, HIGH); digitalWrite(R_IN4, LOW);
  setMotorSpeed(maxSpeed, maxSpeed);
}
void turnRight() {
  digitalWrite(L_IN1, HIGH); digitalWrite(L_IN2, LOW);
  digitalWrite(R_IN3, LOW); digitalWrite(R_IN4, HIGH);
  setMotorSpeed(maxSpeed, maxSpeed);
}
void stopMove() {
  digitalWrite(L_IN1, LOW); digitalWrite(L_IN2, LOW);
  digitalWrite(R_IN3, LOW); digitalWrite(R_IN4, LOW);
  setMotorSpeed(0,0);
}
