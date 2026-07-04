#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define TURBIDITY_PIN A0  // Pin connected to turbidity sensor
#define TDS_PIN A1        // Pin connected to TDS sensor

// Calibration values for turbidity sensor (adjust these based on your tests)
const float rawAir = 600;        // Raw value when the turbidity sensor is in the air (0 NTU)
const float rawClear = 700;      // Raw value for clear drinking water (~0.5-1 NTU)
const float rawLightTurbid = 800; // Raw value for lightly turbid water (5-10 NTU)
const float rawModerate = 850;   // Raw value for moderately polluted water (20-50 NTU)
const float rawWastewater = 900; // Raw value for wastewater (100-500 NTU)
const float rawMilk = 1023;      // Raw value for milk (2000-4000 NTU)

const float ntuAir = 0;          // NTU value when in air
const float ntuClear = 0.5;      // NTU value for clear drinking water
const float ntuLightTurbid = 5;  // NTU value for lightly turbid water
const float ntuModerate = 20;    // NTU value for moderately polluted water
const float ntuWastewater = 300; // NTU value for wastewater
const float ntuMilk = 3000;      // NTU value for milk

// Calibration factor for TDS sensor (adjust according to your sensor's datasheet)
const float tdsCalibrationFactor = 0.5;  // Adjusted for drinking water, closer to real values

void setup() {
  Serial.begin(9600);

  // Initialize OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // 0x3C is a common I2C address for OLED displays
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.display();
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
}

void loop() {
  // Read and calculate turbidity value
  const int numReadingsTurbidity = 100;  // Higher number for better accuracy (was 50)
  long sumTurbidity = 0;

  for (int i = 0; i < numReadingsTurbidity; i++) {
    sumTurbidity += analogRead(TURBIDITY_PIN);
    delay(10); // Shorter delay between readings to stabilize the value
  }

  int rawTurbidity = sumTurbidity / numReadingsTurbidity; // Average raw value for turbidity
  float turbidityNTU = mapRawToNTU(rawTurbidity); // Map raw value to NTU

  // Read and calculate TDS value (average multiple readings)
  const int numReadingsTDS = 100;  // Higher number for better accuracy (was 50)
  long sumTDS = 0;

  for (int i = 0; i < numReadingsTDS; i++) {
    sumTDS += analogRead(TDS_PIN);
    delay(10); // Short delay between readings
  }

  int rawTDS = sumTDS / numReadingsTDS;  // Average raw value for TDS
  float voltage = (rawTDS / 1023.0) * 5.0;  // Convert to voltage (assuming 5V reference)
  float tdsValue = voltage * tdsCalibrationFactor * 1000.0; // Calculate TDS in ppm

  // Display turbidity and TDS values on the Serial Monitor
  Serial.print("Turbidity: ");
  Serial.print(turbidityNTU, 1);  // Display NTU with 1 decimal place
  Serial.print(" NTU\t");

  Serial.print("TDS: ");
  Serial.print(tdsValue, 1);  // Display TDS value in ppm with 1 decimal place
  Serial.println(" ppm");

  // Display values on OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Turbidity: ");
  display.print(turbidityNTU, 1);
  display.println(" NTU");

  display.print("TDS: ");
  display.print(tdsValue, 1);
  display.println(" ppm");

  display.display();

  delay(1000);  // Wait for 1 second before the next loop
}

// Function to map raw turbidity sensor readings to NTU
float mapRawToNTU(int rawValue) {
  if (rawValue <= rawClear) {
    return max(((ntuClear - ntuAir) / (rawClear - rawAir)) * (rawValue - rawAir) + ntuAir, 0);  // Prevent negative NTU
  } else if (rawValue <= rawLightTurbid) {
    return max(((ntuLightTurbid - ntuClear) / (rawLightTurbid - rawClear)) * (rawValue - rawClear) + ntuClear, 0);
  } else if (rawValue <= rawModerate) {
    return max(((ntuModerate - ntuLightTurbid) / (rawModerate - rawLightTurbid)) * (rawValue - rawLightTurbid) + ntuLightTurbid, 0);
  } else if (rawValue <= rawWastewater) {
    return max(((ntuWastewater - ntuModerate) / (rawWastewater - rawModerate)) * (rawValue - rawModerate) + ntuModerate, 0);
  } else {
    return max(((ntuMilk - ntuWastewater) / (rawMilk - rawWastewater)) * (rawValue - rawWastewater) + ntuWastewater, 0);
  }
}
