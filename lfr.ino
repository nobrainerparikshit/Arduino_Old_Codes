#define LEFT_SENSOR 8
#define RIGHT_SENSOR 9
#define ENA 6
#define IN1 2
#define IN2 3
#define ENB 11
#define IN3 4
#define IN4 5
void setup() {pinMode(LEFT_SENSOR, INPUT); pinMode(RIGHT_SENSOR, INPUT); pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT); analogWrite(ENA, 180); analogWrite(ENB, 180);}
void loop() {
  int left = digitalRead(LEFT_SENSOR); int right = digitalRead(RIGHT_SENSOR);
  if (left == HIGH && right == HIGH) {moveForward();}
  else if (left == LOW && right == HIGH) {turnLeft();}
  else if (left == HIGH && right == LOW) {turnRight();}
  else {stopRobot();}
}
void moveForward() {digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);}
void turnLeft() {digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH); digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);}
void turnRight() {digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);}
void stopRobot() {digitalWrite(IN1, LOW); digitalWrite(IN2, LOW); digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);}