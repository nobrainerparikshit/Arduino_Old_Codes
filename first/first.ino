#define MOTOR_SPEED 200

// Right motor
int enableRightMotor = 6;
int rightMotorPin1 = 7;
int rightMotorPin2 = 8;

// Left motor
int enableLeftMotor = 5;
int leftMotorPin1 = 9;
int leftMotorPin2 = 10;

#include <SoftwareSerial.h>
SoftwareSerial Bluetooth(2, 3); // RX, TX

char command;

void setup() {
  // Motor pins setup
  pinMode(enableRightMotor, OUTPUT);
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);
  
  pinMode(enableLeftMotor, OUTPUT);
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);
  
  // Bluetooth setup
  Bluetooth.begin(9600);
  Serial.begin(9600);
  
  // Stop motors at the beginning
  rotateMotor(0, 0);
}

void loop() {
  // Bluetooth control
  if (Bluetooth.available()) {
    command = Bluetooth.read();
    Serial.println(command);
    switch (command) {
      case 'F': // Move forward
        rotateMotor(MOTOR_SPEED, MOTOR_SPEED);
        break;
      case 'B': // Move backward
        rotateMotor(-MOTOR_SPEED, -MOTOR_SPEED);
        break;
      case 'L': // Turn left
        rotateMotor(-MOTOR_SPEED, MOTOR_SPEED);
        break;
      case 'R': // Turn right
        rotateMotor(MOTOR_SPEED, -MOTOR_SPEED);
        break;
      case 'S': // Stop
        rotateMotor(0, 0);
        break;
    }
  }
}

void rotateMotor(int rightMotorSpeed, int leftMotorSpeed) {
  // Control right motor
  if (rightMotorSpeed < 0) {
    digitalWrite(rightMotorPin1, LOW);
    digitalWrite(rightMotorPin2, HIGH);    
  } else if (rightMotorSpeed > 0) {
    digitalWrite(rightMotorPin1, HIGH);
    digitalWrite(rightMotorPin2, LOW);      
  } else {
    digitalWrite(rightMotorPin1, LOW);
    digitalWrite(rightMotorPin2, LOW);      
  }

  // Control left motor
  if (leftMotorSpeed < 0) {
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(leftMotorPin2, HIGH);    
  } else if (leftMotorSpeed > 0) {
    digitalWrite(leftMotorPin1, HIGH);
    digitalWrite(leftMotorPin2, LOW);      
  } else {
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(leftMotorPin2, LOW);      
  }
  
  analogWrite(enableRightMotor, abs(rightMotorSpeed));
  analogWrite(enableLeftMotor, abs(leftMotorSpeed));    
}
