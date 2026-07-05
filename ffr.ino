#define FLAME_SENSOR A0
#define ENA 6
#define IN1 2
#define IN2 3
#define ENB 11
#define IN3 4
#define IN4 5
#define PUMP 7
int flameValue;
void setup() {Serial.begin(9600); pinMode(FLAME_SENSOR, INPUT); pinMode(PUMP, OUTPUT); pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT); analogWrite(ENA, 180); analogWrite(ENB, 180); digitalWrite(PUMP, LOW);}
void loop() {flameValue = analogRead(FLAME_SENSOR); Serial.println(flameValue);
  if (flameValue < 500) {stopRobot(); digitalWrite(PUMP, HIGH); delay(3000); digitalWrite(PUMP, LOW);}
  else {moveForward();}
  delay(100);}
void moveForward() {digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);}
void stopRobot() {digitalWrite(IN1, LOW); digitalWrite(IN2, LOW); digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);}