#include "HX711.h"

#define DT A1
#define SCK A0

HX711 scale;

void setup() {
  Serial.begin(9600);
  scale.begin(DT, SCK);
  
  Serial.println("Initializing the load cell...");
  delay(1000);

  if (!scale.is_ready()) {
    Serial.println("HX711 not found.");
    while (1);
  }

  // Optional: tare (reset to 0)
  scale.set_scale();  // You'll need to find calibration factor later
  scale.tare();       // Reset current reading to 0
  
  Serial.println("Load cell ready! Place a weight.");
}

void loop() {
  Serial.print("Raw reading: ");
  Serial.println(scale.get_units(), 1); // 1 decimal place
  delay(1000);
}
