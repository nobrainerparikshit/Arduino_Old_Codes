#include <WiFiS3.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
char ssid[] = "Parikshit";
char pass[] = "112233440";
WiFiServer server(80);
#define PH_PIN A0
#define TDS_PIN A1
#define TURB_PIN A2
#define RELAY1 7
SoftwareSerial gpsSerial(10,11);
TinyGPSPlus gps;
void setup()
{
  Serial.begin(9600);
  pinMode(RELAY1, OUTPUT);
  digitalWrite(RELAY1, HIGH); 
  gpsSerial.begin(9600);
  Serial.println("Connecting WiFi...");
  while (WiFi.begin(ssid, pass) != WL_CONNECTED)
  {
    Serial.println("Retrying...");
    delay(3000);
  }
  Serial.println("Connected!");
  Serial.println(WiFi.localIP());
  server.begin();
}
void loop()
{
  while (gpsSerial.available())
  {
    gps.encode(gpsSerial.read());
  }
  float lat = gps.location.lat();
  float lon = gps.location.lng();
  int phRaw = analogRead(PH_PIN);
  int tdsRaw = analogRead(TDS_PIN);
  int turbRaw = analogRead(TURB_PIN);
  float phValue = phRaw * (14.0 / 1023.0);
  float tdsValue = tdsRaw * (1000.0 / 1023.0);
  WiFiClient client = server.available();
  if (client)
  {
    String request = client.readStringUntil('\r'); 
    while (client.connected()) {
      if (client.available()) {
        String line = client.readStringUntil('\n');
        if (line == "\r" || line == "") {
          break; 
        }
      }
    }
    if (request.indexOf("/ON") != -1)
    {
      digitalWrite(RELAY1, LOW); 
    }
    if (request.indexOf("/OFF") != -1)
    {
      digitalWrite(RELAY1, HIGH); 
    }
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println("Connection: close");
    client.println();
    client.println("<!DOCTYPE html><html>");
    client.println("<head>");
    client.println("<meta name='viewport' content='width=device-width, initial-scale=1'>");
    client.println("<title>Water Monitoring</title>");
    client.println("<style>");
    client.println("body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background-color: #eef2f5; color: #333; margin: 0; padding: 20px; text-align: center; }");
    client.println("h1 { color: #2c3e50; margin-bottom: 30px; }");
    client.println(".grid-container { display: flex; flex-wrap: wrap; justify-content: center; gap: 20px; max-width: 900px; margin: 0 auto; }");
    client.println(".card { background: white; border-radius: 12px; box-shadow: 0 4px 6px rgba(0,0,0,0.1); padding: 20px; width: 220px; }");
    client.println(".card h2 { margin: 0; font-size: 1rem; color: #7f8c8d; text-transform: uppercase; letter-spacing: 1px; }");
    client.println(".card p { margin: 10px 0 0; font-size: 2rem; font-weight: bold; color: #3498db; }");
    client.println(".controls-section { margin-top: 50px; background: white; padding: 20px; border-radius: 12px; display: inline-block; box-shadow: 0 4px 6px rgba(0,0,0,0.1); }");
    client.println(".btn { border: none; padding: 15px 40px; font-size: 18px; font-weight: bold; color: white; border-radius: 8px; cursor: pointer; text-decoration: none; margin: 10px; display: inline-block; box-shadow: 0 4px 6px rgba(0,0,0,0.2); }");
    client.println(".btn-on { background-color: #27ae60; }");
    client.println(".btn-off { background-color: #c0392b; }");
    client.println("</style>");
    client.println("</head>");
    client.println("<body>");
    client.println("<h1>Water Monitoring Dashboard</h1>");
    client.println("<div class='grid-container'>");
    client.println("<div class='card'><h2>pH Level</h2><p>");
    client.print(phValue);
    client.println("</p></div>");
    client.println("<div class='card'><h2>TDS (ppm)</h2><p>");
    client.print(tdsValue);
    client.println("</p></div>");
    client.println("<div class='card'><h2>Turbidity Raw</h2><p>");
    client.print(turbRaw);
    client.println("</p></div>");
    client.println("<div class='card'><h2>Latitude</h2><p style='font-size: 1.5rem;'>");
    client.print(lat, 5); 
    client.println("</p></div>");
    client.println("<div class='card'><h2>Longitude</h2><p style='font-size: 1.5rem;'>");
    client.print(lon, 5); 
    client.println("</p></div>");
    client.println("</div>"); 
    client.println("<div class='controls-section'>");
    client.println("<h2 style='color: #2c3e50; margin-bottom: 20px;'>Relay Control</h2>");
    client.println("<a href=\"/ON\" class='btn btn-on'>TURN ON</a>");
    client.println("<a href=\"/OFF\" class='btn btn-off'>TURN OFF</a>");
    client.println("</div>");
    client.println("</body></html>");
    client.stop();
  }
}