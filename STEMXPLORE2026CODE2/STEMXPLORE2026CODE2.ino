#include "HUSKYLENS.h"
#include "Wire.h"
#include <Servo.h>
HUSKYLENS huskylens;
#define ENA 5
#define IN1 8
#define IN2 9
#define ENB 6
#define IN3 12
#define IN4 13
Servo servoLeft;
Servo servoRight;
int servoLeftPin = 3;
int servoRightPin = 4;
unsigned long lastMoveTime = 0;
int moveDuration = 2000;
#define SERVO_OPEN_LEFT 90
#define SERVO_OPEN_RIGHT 90
#define SERVO_CLOSE_LEFT 0
#define SERVO_CLOSE_RIGHT 180
void setup()
{
  Serial.begin(9600);
  Wire.begin();
  huskylens.begin(Wire);
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  servoLeft.attach(servoLeftPin);
  servoRight.attach(servoRightPin);
  servoLeft.write(SERVO_OPEN_LEFT);
  servoRight.write(SERVO_OPEN_RIGHT);
  randomSeed(analogRead(A0));
}
void loop()
{
  if (huskylens.request())
  {
    if (huskylens.available())
    {
      HUSKYLENSResult result = huskylens.read();
      stopMotors();
      Serial.println("Object detected");
      servoLeft.write(SERVO_CLOSE_LEFT);
      servoRight.write(SERVO_CLOSE_RIGHT);
      delay(5000);
      servoLeft.write(SERVO_OPEN_LEFT);
      servoRight.write(SERVO_OPEN_RIGHT);
      delay(500);
    }
    else
    {
      randomMovement();
    }
  }
}
void randomMovement()
{
  if (millis() - lastMoveTime > moveDuration)
  {
    int move = random(0,4);
    moveDuration = random(1500,4000);
    lastMoveTime = millis();
    switch(move)
    {
      case 0:
        forward();
        break;

      case 1:
        turnLeft();
        break;

      case 2:
        turnRight();
        break;

      case 3:
        stopMotors();
        break;
    }
  }
}

void forward()
{
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);

  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);

  analogWrite(ENA,200);
  analogWrite(ENB,200);
}

void turnLeft()
{
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);

  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);

  analogWrite(ENA,200);
  analogWrite(ENB,200);
}

void turnRight()
{
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);

  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);

  analogWrite(ENA,200);
  analogWrite(ENB,200);
}

void stopMotors()
{
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,LOW);

  digitalWrite(IN3,LOW);
  digitalWrite(IN4,LOW);
}