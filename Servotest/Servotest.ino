#include <Servo.h>

Servo myServo;
int servoPin = 9;
int stopPWM = 90;
int clockwisePWM = 120;
int counterPWM = 60;    

void setup() {
  Serial.begin(9600);
  myServo.attach(servoPin);
  myServo.write(stopPWM);
  delay(500);
  
  Serial.println("Servo control ready");
}

void loop() {
  moveServoForMs(clockwisePWM, 710);
  delay(1000);
  moveServoForMs(counterPWM, 715);
  delay(1000);
}

void moveServoForMs(int pwmValue, int durationMs) {
  myServo.write(pwmValue);  // Rotate
  delay(durationMs);        // Wait
  myServo.write(stopPWM);   // Stop
}