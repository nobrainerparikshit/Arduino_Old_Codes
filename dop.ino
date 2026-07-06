#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 2
#define PH_SENSOR A0
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
float temperature;
float voltage;
float pH;
float predictedDO;
void setup() {
  Serial.begin(9600);
  sensors.begin();
  pinMode(PH_SENSOR, INPUT);
  Serial.println("AI Water Quality Predictor");
}
void loop() {
  sensors.requestTemperatures();
  temperature = sensors.getTempCByIndex(0);
  int sensorValue = analogRead(PH_SENSOR);
  voltage = sensorValue * (5.0 / 1023.0);
  pH = 7 + ((2.50 - voltage) / 0.18);
  predictedDO = estimateDO(temperature, pH);
  Serial.println("------------------------");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" C");
  Serial.print("pH: ");
  Serial.println(pH,2);
  Serial.print("Predicted DO: ");
  Serial.print(predictedDO,2);
  Serial.println(" mg/L");
  if(predictedDO > 8.0)
    Serial.println("Water Quality: Excellent");
  else if(predictedDO > 6.0)
    Serial.println("Water Quality: Good");
  else if(predictedDO > 4.0)
    Serial.println("Water Quality: Moderate");
  else
    Serial.println("Water Quality: Poor");
  delay(2000);
}
float estimateDO(float temp, float ph){
  float doTemp = 14.6 - (0.40 * temp) + (0.008 * temp * temp);
  float phCorrection = -(abs(ph - 7.0) * 0.15);
  float prediction = doTemp + phCorrection;
  if(prediction < 0)
    prediction = 0;
  if(prediction > 14)
    prediction = 14;
  return prediction;}