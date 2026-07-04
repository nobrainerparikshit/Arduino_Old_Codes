#include <DHT.h>
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
#define mq2Pin A0
#define relayPin 3
#define buzzerPin 4
unsigned long previousMillis = 0;
const long interval = 1000;
bool emergencyActive = false;
unsigned long emergencyStart = 0;
const long emergencyTime = 10000; 
void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(relayPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  digitalWrite(buzzerPin, LOW);
}
void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    int gas = analogRead(mq2Pin);
    float temp = dht.readTemperature();

    Serial.print("Gas: "); Serial.println(gas);
    Serial.print("Temp: "); Serial.println(temp);

    
    if ((gas > 150 || temp > 40) && !emergencyActive) {
      emergencyStart = currentMillis;
      emergencyActive = true;
      digitalWrite(buzzerPin, HIGH);
      digitalWrite(relayPin, HIGH);
    }
  }

  if (emergencyActive && (millis() - emergencyStart >= emergencyTime)) {
    emergencyActive = false;
    digitalWrite(buzzerPin, LOW);
    digitalWrite(relayPin, LOW);
  }
}