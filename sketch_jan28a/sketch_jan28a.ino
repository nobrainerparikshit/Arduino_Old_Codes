#define BLYNK_TEMPLATE_ID "TMPL3BV8H1C-H"
#define BLYNK_TEMPLATE_NAME "SERVO CONTROL"
#define BLYNK_AUTH_TOKEN "xfYcvLqfzhtpQs353J86kIV_7Wp7RfYt"

#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>

char auth[] = BLYNK_AUTH_TOKEN;  // Blynk authentication token
char ssid[] = "Harshdev";        // Your WiFi SSID
char pass[] = "12345678";        // Your WiFi password

Servo servo1;                    // Declare one servo object
int currentPosition = 93;        // Default starting position of the servo (middle)

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);  // Connect to Blynk
  servo1.attach(13);              // Attach servo to GPIO 13
  servo1.write(currentPosition);  // Set servo to the default position
}

void loop() {
  Blynk.run();  // Run the Blynk library
}

// Slider Widget on V0 (Forward: 93 to 180)
BLYNK_WRITE(V0) {
  int pos1 = param.asInt();  // Get the value from the slider (93 to 180)
  if (pos1 >= 93 && pos1 <= 180) {
    currentPosition = pos1;   // Update the current position
    servo1.write(pos1);       // Move the servo
    Blynk.virtualWrite(V2, 93 - (pos1 - 93)); // Update reverse slider (V2)
  }
}

// Slider Widget on V2 (Reverse: 0 to 93)
BLYNK_WRITE(V2) {
  int pos2 = param.asInt();  // Get the value from the reverse slider (0 to 93)
  if (pos2 >= 0 && pos2 <= 93) {
    currentPosition = pos2;   // Update the current position
    servo1.write(pos2);       // Move the servo
    Blynk.virtualWrite(V0, 180 - (pos2 + 93)); // Update forward slider (V0)
  }
}
