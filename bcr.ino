char command;
#define ENA 6
#define IN1 2
#define IN2 3
#define ENB 11
#define IN3 4
#define IN4 5
void setup() {
  Serial.begin(9600);
  pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  analogWrite(ENA, 200); analogWrite(ENB, 200);
  stopRobot();}
void loop() {
  if (Serial.available()) {
    command = Serial.read();
    switch (command) {
      case 'F':
        moveForward(); break;
      case 'B':
        moveBackward(); break;
      case 'L':
        turnLeft(); break;
      case 'R':
        turnRight(); break;
      case 'S':
        stopRobot(); break;
    }
  }
}
void moveForward() {digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);}
void moveBackward() {digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH); digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);}
void turnLeft() {digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH); digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);}
void turnRight() {digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);}
void stopRobot() {digitalWrite(IN1, LOW); digitalWrite(IN2, LOW); digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);}