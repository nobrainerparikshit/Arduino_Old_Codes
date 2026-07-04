#include "HUSKYLENS.h"
#include <Wire.h>

HUSKYLENS huskylens;

void setup() {
    Serial.begin(115200);
    Wire.begin();
    if (!huskylens.begin(Wire)) {
        Serial.println("HuskyLens initialization failed!");
        while (1);
    }
    huskylens.writeAlgorithm(ALGORITHM_OBJECT_TRACKING); // Set to object tracking mode
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    if (!huskylens.request()) {
        Serial.println("Failed to request data from HuskyLens");
        return;
    }
    
    bool objectDetected = false;
    for (int i = 0; i < huskylens.count(); i++) {
        HUSKYLENSResult result = huskylens.get(i);
        if (result.ID == 1) { // Check if Object 1 is detected
            objectDetected = true;
            break;
        }
    }
    
    if (objectDetected) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(500);
        digitalWrite(LED_BUILTIN, LOW);
        delay(500);
    } else {
        digitalWrite(LED_BUILTIN, LOW);
    }
}