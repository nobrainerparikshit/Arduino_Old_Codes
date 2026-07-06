#define RAIN_SENSOR A0
#define ENA 6
#define IN1 2
#define IN2 3
#define ENB 11
#define IN3 4
#define IN4 5
void setup() {Serial.begin(9600); pinMode(RAIN_SENSOR, INPUT); pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  analogWrite(ENA, 180); analogWrite(ENB, 180);}
void loop() {int rain = analogRead(RAIN_SENSOR);
  Serial.println(rain);
  if (rain < 500) {stopRobot();}
  else {moveForward();}
  delay(100);}
void moveForward() {digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);}
void stopRobot() {digitalWrite(IN1, LOW); digitalWrite(IN2, LOW); digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);}