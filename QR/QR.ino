#include <WiFiS3.h>
WiFiServer server(80);

char ssid[] = "Harshdev";
char pass[] = "12345678";

// Your actual website link
const char* websiteLink = "https://9000-firebase-studio-1747987865652.cluster-w5vd22whf5gmav2vgkomwtc4go.cloudworkstations.dev";

void setup() {
  Serial.begin(9600);

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.println("Connecting to WiFi...");
    delay(2000);
  }

  Serial.println("Connected to WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    while (!client.available()) delay(1);

    // Create the full QR code URL using the API
    String qrUrl = "https://api.qrserver.com/v1/create-qr-code/?size=150x150&data=https://9000-firebase-studio-1747987865652.cluster-w5vd22whf5gmav2vgkomwtc4go.cloudworkstations.dev";
    qrUrl += websiteLink;

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    client.println("<!DOCTYPE html><html><body>");
    client.println("<h2>Scan this QR Code to open the website:</h2>");
    client.print("<img src='");
    client.print(qrUrl);
    client.println("' alt='QR Code'><br><br>");
    client.print("<p>Link: <a href='");
    client.print(websiteLink);
    client.print("'>");
    client.print(websiteLink);
    client.println("</a></p>");
    client.println("</body></html>");

    delay(1);
    client.stop();
  }
}
