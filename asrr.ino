#include <Servo.h>

#define TRIG_PIN 9
#define ECHO_PIN 10

#define FLAME_SENSOR A0
#define RAIN_SENSOR A1

#define PUMP 7

#define ENA 6
#define IN1 2
#define IN2 3

#define ENB 11
#define IN3 4
#define IN4 5

Servo scanner;

long duration;
int distance;
int flameValue;
int rainValue;

void setup() {

  Serial.begin(9600);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(FLAME_SENSOR, INPUT);
  pinMode(RAIN_SENSOR, INPUT);

  pinMode(PUMP, OUTPUT);

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  analogWrite(ENA,180);
  analogWrite(ENB,180);

  digitalWrite(PUMP,LOW);

  scanner.attach(8);
  scanner.write(90);
}

void loop() {

  distance = getDistance();
  flameValue = analogRead(FLAME_SENSOR);
  rainValue = analogRead(RAIN_SENSOR);

  Serial.print("Distance: ");
  Serial.print(distance);

  Serial.print(" Flame: ");
  Serial.print(flameValue);

  Serial.print(" Rain: ");
  Serial.println(rainValue);

  if(flameValue < 450){

    stopRobot();

    digitalWrite(PUMP,HIGH);

    delay(3000);

    digitalWrite(PUMP,LOW);

    return;
  }

  if(rainValue < 500){

    stopRobot();

    delay(500);

    return;
  }

  if(distance > 25){

    moveForward();
  }

  else{

    stopRobot();

    delay(300);

    scanEnvironment();
  }

  delay(100);
}

int getDistance(){

  digitalWrite(TRIG_PIN,LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN,HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN,LOW);

  duration = pulseIn(ECHO_PIN,HIGH);

  return duration * 0.034 / 2;
}

void scanEnvironment(){

  scanner.write(30);
  delay(500);
  int leftDistance = getDistance();

  scanner.write(150);
  delay(500);
  int rightDistance = getDistance();

  scanner.write(90);
  delay(300);

  if(leftDistance > rightDistance){

    turnLeft();

    delay(600);
  }

  else{

    turnRight();

    delay(600);
  }

  stopRobot();
}

void moveForward(){

  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);

  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
}

void moveBackward(){

  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);

  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);
}

void turnLeft(){

  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);

  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
}

void turnRight(){

  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);

  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);
}

void stopRobot(){

  digitalWrite(IN1,LOW);
  digitalWrite(IN2,LOW);

  digitalWrite(IN3,LOW);
  digitalWrite(IN4,LOW);
}