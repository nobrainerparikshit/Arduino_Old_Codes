#include <WiFiS3.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <OneWire.h>
#include <DallasTemperature.h>
char ssid[] = "Parikshit";
char pass[] = "112233444";
WiFiServer server(80);
#define PH_PIN A0
#define TDS_PIN A1
#define TURB_PIN A2
#define RELAY1 7
#define ONE_WIRE_BUS 2
SoftwareSerial gpsSerial(10,11);
TinyGPSPlus gps;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
float tempArr[20], phArr[20], tdsArr[20], turbArr[20];
int indexData = 0;
void setup() {
  Serial.begin(9600);
  pinMode(RELAY1, OUTPUT);
  digitalWrite(RELAY1, HIGH);
  gpsSerial.begin(9600);
  sensors.begin();
  WiFi.disconnect();
  delay(1000);
  Serial.println("Connecting WiFi...");
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.println("Retrying...");
    delay(4000);
  }
  Serial.println("Connected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  server.begin();
}
void loop() {
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }
  float lat = gps.location.lat();
  float lon = gps.location.lng();
  int phRaw = analogRead(PH_PIN);
  int tdsRaw = analogRead(TDS_PIN);
  int turbRaw = analogRead(TURB_PIN);
  float phValue = phRaw * (14.0 / 1023.0);
  float tdsValue = tdsRaw * (1000.0 / 1023.0);
  float turbValue = turbRaw * (50.0 / 1023.0);
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);
  tempArr[indexData] = tempC;
  phArr[indexData] = phValue;
  tdsArr[indexData] = tdsValue;
  turbArr[indexData] = turbValue;
  indexData = (indexData + 1) % 20;
  delay(1000);
  WiFiClient client = server.available();
  if (client) {
    String request = client.readStringUntil('\r');
    if (request.indexOf("/ON") != -1) digitalWrite(RELAY1, LOW);
    if (request.indexOf("/OFF") != -1) digitalWrite(RELAY1, HIGH);
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println();
    client.println("<html><head>");
    client.println("<meta http-equiv='refresh' content='2'>");
    client.println("<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>");
    client.println("<style>");
    client.println("body { font-family: Arial; background:#eef2f5; text-align:center; margin:0; padding:20px; }");
    client.println("h1 { color:#2c3e50; }");
    client.println(".data-box { background:white; padding:15px; margin:10px auto; border-radius:10px; width:260px; box-shadow:0 2px 5px rgba(0,0,0,0.1); }");
    client.println(".btn { padding:10px 20px; margin:10px; border:none; border-radius:6px; color:white; font-weight:bold; cursor:pointer; }");
    client.println(".on { background:green; }");
    client.println(".off { background:red; }");
    client.println(".graph-container { display:grid; grid-template-columns: repeat(2,1fr); gap:15px; max-width:520px; margin:20px auto; }");
    client.println(".graph-box { background:white; padding:10px; border-radius:10px; box-shadow:0 2px 5px rgba(0,0,0,0.1); }");
    client.println(".graph-box canvas { width:100% !important; height:200px !important; }");
    client.println("</style>");
    client.println("</head><body>");
    client.println("<h1>Water Monitoring Dashboard</h1>");
    client.println("<div class='data-box'>");
    client.print("Temperature: "); client.print(tempC); client.println(" °C<br>");
    client.print("pH: "); client.print(phValue); client.println("<br>");
    client.print("TDS: "); client.print(tdsValue); client.println(" ppm<br>");
    client.print("Turbidity: "); client.print(turbValue); client.println(" NTU<br>");
    client.print("Latitude: "); client.print(lat,5); client.println("<br>");
    client.print("Longitude: "); client.print(lon,5);
    client.println("</div>");
    client.println("<a href='/ON'><button class='btn on'>Relay ON</button></a>");
    client.println("<a href='/OFF'><button class='btn off'>Relay OFF</button></a>");
    client.println("<div class='graph-container'>");
    client.println("<div class='graph-box'><h4>Temperature</h4><canvas id='tempChart'></canvas></div>");
    client.println("<div class='graph-box'><h4>pH</h4><canvas id='phChart'></canvas></div>");
    client.println("<div class='graph-box'><h4>TDS</h4><canvas id='tdsChart'></canvas></div>");
    client.println("<div class='graph-box'><h4>Turbidity</h4><canvas id='turbChart'></canvas></div>");
    client.println("</div>");
    client.println("<script>");
    client.print("var tempData=[");
    for(int i=0;i<20;i++){ client.print(tempArr[i]); if(i<19) client.print(","); }
    client.println("];");
    client.print("var phData=[");
    for(int i=0;i<20;i++){ client.print(phArr[i]); if(i<19) client.print(","); }
    client.println("];");
    client.print("var tdsData=[");
    for(int i=0;i<20;i++){ client.print(tdsArr[i]); if(i<19) client.print(","); }
    client.println("];");
    client.print("var turbData=[");
    for(int i=0;i<20;i++){ client.print(turbArr[i]); if(i<19) client.print(","); }
    client.println("];");
    client.println("function makeChart(id,data,label,max){");
    client.println("new Chart(document.getElementById(id),{type:'line',data:{labels:data.map((_,i)=>i),datasets:[{label:label,data:data}]},options:{responsive:true,plugins:{legend:{display:false}},scales:{y:{min:0,max:max}}}});}");
    client.println("makeChart('tempChart',tempData,'Temp',100);");
    client.println("makeChart('phChart',phData,'pH',14);");
    client.println("makeChart('tdsChart',tdsData,'TDS',1000);");
    client.println("makeChart('turbChart',turbData,'Turbidity',50);");
    client.println("</script>");
    client.println("</body></html>");
    client.stop();
  }
}