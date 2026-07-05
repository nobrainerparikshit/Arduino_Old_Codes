#include <Wire.h>
#include <MPU6050.h>
MPU6050 mpu;
#define ENA 6
#define IN1 2
#define IN2 3
#define ENB 11
#define IN3 4
#define IN4 5
int16_t ax, ay, az;
void setup() {Serial.begin(9600); Wire.begin(); mpu.initialize(); pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT); analogWrite(ENA, 180); analogWrite(ENB, 180); stopRobot();}
void loop() {mpu.getAcceleration(&ax, &ay, &az); Serial.print("X: "); Serial.print(ax); Serial.print("  Y: "); Serial.println(ay);
  if (ay > 8000) {moveForward();}
  else if (ay < -8000) {moveBackward();}
  else if (ax > 8000) {turnRight();}
  else if (ax < -8000) {turnLeft();}
  else {stopRobot();}
  delay(100);}
void moveForward() {digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);}
void moveBackward() {digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH); digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);}
void turnLeft() {digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH); digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);}
void turnRight() {digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);}
void stopRobot() {digitalWrite(IN1, LOW); digitalWrite(IN2, LOW); digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);}