#include <HUSKYLENS.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <math.h>
#include <QMC5883LCompass.h>
#include <TinyGPSPlus.h>

// Motor control pins
const int L_IN1 = 22, L_IN2 = 23, L_ENA = 5;
const int R_IN3 = 24, R_IN4 = 25, R_ENB = 6;
const int maxSpeed = 190;

// Sensor pins
const int pHPin = A0;
const int TDSPin = A1;
const int turbidityPin = A2;

// Relay pin
const int relayPin = 7;
unsigned long relayTimer = 0;
const unsigned long relayDuration = 3000;

// Modules
HUSKYLENS huskylens;
SoftwareSerial BT(10, 11);     // Bluetooth
SoftwareSerial gpsSerial(12, 13);  // GPS: RX = 12, TX = 13
QMC5883LCompass compass;
TinyGPSPlus gps;

void setup() {
  Serial.begin(115200);
  BT.begin(9600);
  gpsSerial.begin(9600);

  // Motor pins
  pinMode(L_IN1, OUTPUT); pinMode(L_IN2, OUTPUT); pinMode(L_ENA, OUTPUT);
  pinMode(R_IN3, OUTPUT); pinMode(R_IN4, OUTPUT); pinMode(R_ENB, OUTPUT);
  stopMove();

  // Relay
  pinMode(relayPin, OUTPUT);

  // I2C init
  Wire.begin();

  // HuskyLens setup
  while (!huskylens.begin(Wire)) {
    Serial.println("HuskyLens I2C fail; check connections.");
    delay(500);
  }
  huskylens.writeAlgorithm(ALGORITHM_COLOR_RECOGNITION);
  Serial.println("HuskyLens ready!");

  // Magnetometer setup
  compass.init();
  compass.setCalibration(-1780, 1988, -1716, 1576, -2360, 2136);
  Serial.println("Magnetometer ready!");

  Serial.println("GPS ready!");
}

void loop() {
  // Read analog sensors
  float voltagePH = analogRead(pHPin) * (5.0 / 1023.0);
  float voltageTDS = analogRead(TDSPin) * (5.0 / 1023.0);
  float voltageTurbidity = analogRead(turbidityPin) * (5.0 / 1023.0);

  float pHValue = 7.0 + (2.5 - voltagePH) / 0.18;
  float tdsValue = (133.42 * pow(voltageTDS, 3) - 255.86 * pow(voltageTDS, 2) + 857.39 * voltageTDS) * 0.5;
  float turbidityValue = (5.0 - voltageTurbidity) * 100.0;  // approx scale

  // Relay alert check
  bool alert = (pHValue > 8.5 || tdsValue > 300.0 || turbidityValue > 50.0);

  if (alert && relayTimer == 0) {
    digitalWrite(relayPin, HIGH);
    relayTimer = millis();
  }
  if (relayTimer != 0 && millis() - relayTimer >= relayDuration) {
    digitalWrite(relayPin, LOW);
    relayTimer = 0;
  }

  // HuskyLens color detection
  if (huskylens.request()) {
    bool greenDetected = false;
    while (huskylens.available()) {
      HUSKYLENSResult result = huskylens.read();
      if (result.ID == 1) {
        greenDetected = true;
        break;
      }
    }
    if (greenDetected) {
      Serial.println("Green color detected. Moving forward.");
      moveForward();
    } else {
      Serial.println("No green detected. Stopping.");
      stopMove();
    }
  } else {
    Serial.println("HuskyLens request failed.");
    stopMove();
  }

  // Magnetometer heading
  compass.read();
  int heading = compass.getAzimuth();

  // GPS reading
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  String gpsData = "GPS: ";
  if (gps.location.isValid()) {
    gpsData += "Lat: " + String(gps.location.lat(), 6);
    gpsData += " | Lng: " + String(gps.location.lng(), 6);
  } else {
    gpsData += "No fix";
  }

  // Send all data
  String data = "pH: " + String(pHValue, 2) + " | TDS: " + String(tdsValue, 1) + " ppm | Turbidity: " + String(turbidityValue, 1);
  data += " | Heading: " + String(heading);
  data += " | " + gpsData;

  Serial.println(data);
  BT.println(data);

  delay(1000);
}

// --- Motor Control ---
void setMotorSpeed(int l, int r) {
  analogWrite(L_ENA, l);
  analogWrite(R_ENB, r);
}

void moveForward() {
  digitalWrite(L_IN1, HIGH); digitalWrite(L_IN2, LOW);
  digitalWrite(R_IN3, HIGH); digitalWrite(R_IN4, LOW);
  setMotorSpeed(maxSpeed, maxSpeed);
}

void moveBackward() {
  digitalWrite(L_IN1, LOW); digitalWrite(L_IN2, HIGH);
  digitalWrite(R_IN3, LOW); digitalWrite(R_IN4, HIGH);
  setMotorSpeed(maxSpeed, maxSpeed);
}

void turnLeft() {
  digitalWrite(L_IN1, LOW); digitalWrite(L_IN2, HIGH);
  digitalWrite(R_IN3, HIGH); digitalWrite(R_IN4, LOW);
  setMotorSpeed(maxSpeed, maxSpeed);
}

void turnRight() {
  digitalWrite(L_IN1, HIGH); digitalWrite(L_IN2, LOW);
  digitalWrite(R_IN3, LOW); digitalWrite(R_IN4, HIGH);
  setMotorSpeed(maxSpeed, maxSpeed);
}

void stopMove() {
  digitalWrite(L_IN1, LOW); digitalWrite(L_IN2, LOW);
  digitalWrite(R_IN3, LOW); digitalWrite(R_IN4, LOW);
  setMotorSpeed(0, 0);
}