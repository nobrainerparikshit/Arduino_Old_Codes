void setup() {
    Serial.begin(9600); // Start serial communication
    pinMode(A0, INPUT); // Set A0 as input
}

void loop() {
    int rawValue = analogRead(A0); // Read raw analog value
    float voltage = (rawValue * 5.0) / 1024.0; // Convert raw value to voltage
    float calibration_value = 7.0; // Adjust based on your buffer solution calibration
    float pH = -5.7 * voltage + calibration_value; // Calculate pH value

    // Print debug information
    Serial.print("Raw Value: ");
    Serial.println(rawValue);
    Serial.print("Voltage: ");
    Serial.println(voltage);
    Serial.print("pH Value: ");
    Serial.println(pH);
    Serial.println("--------------------");

    delay(1000); // Delay for readability in Serial Monitor
}
