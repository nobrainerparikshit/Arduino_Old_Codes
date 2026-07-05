#include <HUSKYLENS.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <math.h>

const int L_IN1 = 22, L_IN2 = 23, L_ENA = 5;
const int R_IN3 = 24, R_IN4 = 25, R_ENB = 6;
const int maxSpeed = 190;
const int relayPin = 7, buzzerPin = 8;
const int pHPin = A0;
const int TDSPin = A1;
unsigned long relayTimer = 0;
const unsigned long relayDuration = 3000;
HUSKYLENS huskylens;
const int objWidth = 50;
bool objectDetectedClose = false;
SoftwareSerial BT(10, 11);
unsigned long lastRandomMoveTime = 0;
const unsigned long randomInterval = 4000;

void setup() {
  Serial.begin(115200);
  BT.begin(9600);
  pinMode(L_IN1, OUTPUT); pinMode(L_IN2, OUTPUT); pinMode(L_ENA, OUTPUT);
  pinMode(R_IN3, OUTPUT); pinMode(R_IN4, OUTPUT); pinMode(R_ENB, OUTPUT);
  pinMode(relayPin, OUTPUT);**
  pinMode(buzzerPin, OUTPUT);
  stopMove();
  Wire.begin();
  while (!huskylens.begin(Wire)) {
    Serial.println("HuskyLens I2C fail; check connections.");
    delay(200);
  }
  huskylens.writeAlgorithm(ALGORITHM_OBJECT_TRACKING);
  Serial.println("HuskyLens ready!");
}

void loop() {
  int rawPH = analogRead(pHPin);
  int rawTDS = analogRead(TDSPin);
  float voltagePH = rawPH * (5.0 / 1023.0);
  float voltageTDS = rawTDS * (5.0 / 1023.0);
  float pHValue = 7.0 + (2.5 - voltagePH) / 0.18;
  float tdsValue = (133.42 * pow(voltageTDS, 3) - 255.86 * pow(voltageTDS, 2) + 857.39 * voltageTDS) * 0.5;
  String data = "pH: " + String(pHValue, 2) + " | TDS: " + String(tdsValue, 1) + " ppm";
  Serial.println(data);
  BT.println(data);

  bool alert = false;
  if (pHValue > 8.5 || tdsValue > 300.0) alert = true;
  if (alert && relayTimer == 0) {
    digitalWrite(relayPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
    relayTimer = millis();
  }

  if (relayTimer != 0 && millis() - relayTimer >= relayDuration) {
    digitalWrite(relayPin, LOW);
    digitalWrite(buzzerPin, LOW);
    relayTimer = 0;
  }

  if (huskylens.request() && huskylens.available()) {
    HUSKYLENSResult r = huskylens.read();
    BT.print("Object: x="); BT.print(r.xCenter);
    BT.print(" | w="); BT.println(r.width);
    trackObject(r.xCenter, r.width);
  } else {
    randomRoam();
  }
}

void trackObject(int xCenter, int width) {
  if (xCenter < 140) {
    turnRight();
  }
  else if (xCenter > 180) {
    turnLeft();
  }
  else {
    if (width < objWidth) {
      turnRight();
    } else {
      stopMove();
    }
  }
  if (width > objWidth + 20) {
    digitalWrite(relayPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
    relayTimer = relayTimer == 0 ? millis() : relayTimer;
    objectDetectedClose = true;
  } else {
    digitalWrite(relayPin, LOW);
    digitalWrite(buzzerPin, LOW);
    objectDetectedClose = false;
  }
}

void randomRoam() {
  if (millis() - lastRandomMoveTime >= randomInterval) {
    int action = random(0, 3);
    switch (action) {
      case 0: turnLeft(); break;
      case 1: turnRight(); break;
      case 2: stopMove(); break;
    }
    lastRandomMoveTime = millis();
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
  setMotorSpeed(0, 0);
}