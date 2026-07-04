#include <Wire.h>
#include "HUSKYLENS.h"

HUSKYLENS huskylens;

const int L_IN1 = 22, L_IN2 = 23, L_ENA = 9;
const int R_IN3 = 24, R_IN4 = 25, R_ENB = 10;

const int ALGAE_ID = 1;

enum Mode { PATROL, CHARGE };
Mode mode = PATROL;

unsigned long tRandom = 0;
unsigned long dRandom = 0;
unsigned long tCharge = 0;

int lastAction = -1;
int patrolSpeed = 180;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  while (!huskylens.begin(Wire)) {
    Serial.println("HuskyLens not connected");
    delay(300);
  }
  Serial.println("HuskyLens connected");
  pinMode(L_IN1, OUTPUT);
  pinMode(L_IN2, OUTPUT);
  pinMode(L_ENA, OUTPUT);
  pinMode(R_IN3, OUTPUT);
  pinMode(R_IN4, OUTPUT);
  pinMode(R_ENB, OUTPUT);
  randomSeed(analogRead(A0));
  stopMotors();
  Serial.println("State: PATROL");
}

void loop() {
  bool detected = false;

  if (huskylens.request()) {
    while (huskylens.available()) {
      HUSKYLENSResult r = huskylens.read();
      if (r.command == COMMAND_RETURN_BLOCK && r.ID == ALGAE_ID) detected = true;
    }
  } else {
    Serial.println("HuskyLens request failed");
  }

  if (mode == CHARGE) {
    if (millis() - tCharge < 5000) {
      if (lastAction != 1) Serial.println("Action: CHARGE FORWARD");
      forward(220);
      lastAction = 1;
    } else {
      stopMotors();
      lastAction = 0;
      mode = PATROL;
      Serial.println("Charge complete");
      Serial.println("State: PATROL");
      tRandom = 0;
    }
    return;
  }

  if (detected) {
    Serial.println("Detected: ID=1");
    mode = CHARGE;
    tCharge = millis();
    return;
  }

  doPatrol();
}

void doPatrol() {
  if (millis() - tRandom >= dRandom) {
    tRandom = millis();
    dRandom = random(900, 1800);
    patrolSpeed = random(160, 221);
    int act = random(1, 5);
    if (act == 1) {
      if (lastAction != 1) Serial.println("Patrol: FORWARD");
      forward(patrolSpeed);
      lastAction = 1;
    } else if (act == 2) {
      if (lastAction != 2) Serial.println("Patrol: BACKWARD");
      backward(patrolSpeed);
      lastAction = 2;
    } else if (act == 3) {
      if (lastAction != 3) Serial.println("Patrol: LEFT");
      left(patrolSpeed);
      lastAction = 3;
    } else {
      if (lastAction != 4) Serial.println("Patrol: RIGHT");
      right(patrolSpeed);
      lastAction = 4;
    }
  }
}

void forward(int s) {
  digitalWrite(L_IN1, HIGH);
  digitalWrite(L_IN2, LOW);
  digitalWrite(R_IN3, HIGH);
  digitalWrite(R_IN4, LOW);
  analogWrite(L_ENA, s);
  analogWrite(R_ENB, s);
}

void backward(int s) {
  digitalWrite(L_IN1, LOW);
  digitalWrite(L_IN2, HIGH);
  digitalWrite(R_IN3, LOW);
  digitalWrite(R_IN4, HIGH);
  analogWrite(L_ENA, s);
  analogWrite(R_ENB, s);
}

void left(int s) {
  digitalWrite(L_IN1, LOW);
  digitalWrite(L_IN2, HIGH);
  digitalWrite(R_IN3, HIGH);
  digitalWrite(R_IN4, LOW);
  analogWrite(L_ENA, s);
  analogWrite(R_ENB, s);
}

void right(int s) {
  digitalWrite(L_IN1, HIGH);
  digitalWrite(L_IN2, LOW);
  digitalWrite(R_IN3, LOW);
  digitalWrite(R_IN4, HIGH);
  analogWrite(L_ENA, s);
  analogWrite(R_ENB, s);
}

void stopMotors() {
  digitalWrite(L_IN1, LOW);
  digitalWrite(L_IN2, LOW);
  digitalWrite(R_IN3, LOW);
  digitalWrite(R_IN4, LOW);
  analogWrite(L_ENA, 0);
  analogWrite(R_ENB, 0);
  Serial.println("Action: STOP");
}
