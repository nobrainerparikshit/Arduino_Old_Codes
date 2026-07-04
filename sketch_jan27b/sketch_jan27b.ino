#include <ESP32Servo.h>
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;
Servo myServo;

int currentPosition = 90; // Initial servo position (neutral)
unsigned long startTime = 0; // To track rotation time
bool isRotating = false; // Flag to track if the servo is currently rotating
char lastCommand = '0'; // To store the last command ('1' or '2')

void setup() {
  Serial.begin(9600);
  SerialBT.begin("ESP32_Servo"); // Set your Bluetooth name
  myServo.attach(13); // Attach the servo to GPIO 13
  myServo.write(currentPosition); // Set the servo to the neutral position
  Serial.println("Bluetooth device is ready to pair");
}

void loop() {
  // Check for new Bluetooth commands
  if (SerialBT.available()) {
    char command = SerialBT.read();
    Serial.println(command);

    // Handle the '1' or '2' commands
    if ((command == '1' || command == '2') && !isRotating) {
      lastCommand = command; // Store the command
      isRotating = true; // Set the rotation flag
      startTime = millis(); // Record the start time
      Serial.print("Command received: ");
      Serial.println(command);
    }
  }

  // Handle servo rotation logic
  if (isRotating) {
    if (millis() - startTime < 2000) { // Rotate only for 2 seconds
      if (lastCommand == '1') {
        // Rotate incrementally to the right
        currentPosition += 2; // Increment faster to prevent slow movement
        if (currentPosition > 180) currentPosition = 180; // Cap at 180 degrees
      } else if (lastCommand == '2') {
        // Rotate incrementally to the left
        currentPosition -= 2; // Decrement faster to prevent slow movement
        if (currentPosition < 0) currentPosition = 0; // Cap at 0 degrees
      }
      myServo.write(currentPosition); // Update the servo position
      delay(15); // Smooth movement
    } else {
      // Stop rotating after 2 seconds
      isRotating = false; // Reset the rotation flag
      Serial.print("Stopped at position: ");
      Serial.println(currentPosition);
    }
  }
}