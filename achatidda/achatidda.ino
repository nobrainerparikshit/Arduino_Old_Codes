#include <WiFiS3.h>
#include <TinyGPSPlus.h>
#include <Wire.h>
#include <QMC5883LCompass.h>
const char* WIFI_SSID = "Relay";
const char* WIFI_PASS = "shubh110110pp";
WiFiServer server(80);
const int PH_PIN   = A0;
const int TURB_PIN = A2;
const int TDS_PIN  = A1;
const int RELAY_PIN = 7;
const float VREF    = 5.0;
const int   ADC_MAX = 4095;
TinyGPSPlus gps;
QMC5883LCompass compass;
bool relayState = false; 
struct LogEntry {
  unsigned long ms;
  float ph, ntu, tds;
  float heading;
  double lat, lng;
  bool hasFix;
};
const int LOG_CAP = 60;
LogEntry logBuf[LOG_CAP];
int logCount = 0;
int logHead  = 0;
unsigned long lastSampleMs = 0;
unsigned long lastLogMs    = 0;
const unsigned long SAMPLE_PERIOD_MS = 1000;
const unsigned long LOG_PERIOD_MS    = 2000;
float PH_V_NEUTRAL = 2.50f;
float PH_SLOPE     = -5.70f;
const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<title>Water Quality Dashboard</title>
<meta name="viewport" content="width=device-width,initial-scale=1">
<link rel="stylesheet" href="https://unpkg.com/leaflet@1.9.4/dist/leaflet.css">
<script src="https://unpkg.com/leaflet@1.9.4/dist/leaflet.js"></script>
<script src="https://cdn.jsdelivr.net/npm/chart.js@4.4.1/dist/chart.umd.min.js"></script>
<style>
:root{--bg:#0f172a;--card:#111827;--ink:#e5e7eb;--accent:#06b6d4;--good:#10b981;--warn:#f59e0b;--bad:#ef4444}
*{box-sizing:border-box}
body{margin:0;font-family:ui-sans-serif,system-ui,Segoe UI,Roboto,Arial;color:var(--ink);background:radial-gradient(1200px 600px at 100% 0%,rgba(6,182,212,.18),transparent 60%),radial-gradient(900px 500px at 0% 100%,rgba(16,185,129,.15),transparent 60%),var(--bg);}
header{padding:18px 16px;border-bottom:1px solid #1f2937;display:flex;gap:10px;align-items:center;justify-content:space-between}
.brand{font-weight:700;letter-spacing:.3px}
.ip{opacity:.7;font-size:.9rem}
.wrap{max-width:1100px;margin:0 auto;padding:18px}
.grid{display:grid;gap:14px;grid-template-columns:repeat(auto-fit,minmax(220px,1fr))}
.card{background:var(--card);border:1px solid #1f2937;border-radius:16px;padding:16px;box-shadow:0 10px 30px rgba(0,0,0,.25)}
.label{opacity:.8;font-size:.9rem;margin-bottom:8px}
.value{font-size:1.9rem;font-weight:700}
.unit{opacity:.7;font-size:.9rem;margin-left:4px}
.status{margin-top:6px;padding:6px 10px;border-radius:999px;font-size:.8rem;display:inline-block}
.good{background:rgba(16,185,129,.15);color:#34d399}
.warn{background:rgba(245,158,11,.15);color:#fbbf24}
.bad{background:rgba(239,68,68,.15);color:#f87171}
.row{display:grid;gap:14px;grid-template-columns:1.2fr .8fr}
#map{height:350px;border-radius:16px;border:1px solid #1f2937;overflow:hidden}
.compassWrap{display:flex;align-items:center;gap:16px}
.compass{width:96px;height:96px;border-radius:50%;border:2px solid #1f2937;position:relative;background:conic-gradient(from 0deg,#1f2937,#111827)}
.needle{position:absolute;left:50%;top:50%;width:2px;height:44px;background:var(--accent);transform-origin:bottom center;transform:translate(-50%,-100%) rotate(0deg);box-shadow:0 0 10px rgba(6,182,212,.6)}
.charts{display:grid;gap:14px;grid-template-columns:repeat(auto-fit,minmax(260px,1fr))}
.btnbar{display:flex;gap:10px;flex-wrap:wrap;margin-top:10px}
.btn{background:linear-gradient(135deg,#06b6d4,#3b82f6);border:none;color:white;padding:10px 14px;border-radius:10px;cursor:pointer}
.btn.red{background:linear-gradient(135deg, #ef4444, #dc2626)}
.btn:hover{filter:brightness(1.05)}
footer{opacity:.6;text-align:center;padding:18px}
a{color:#67e8f9;text-decoration:none}
</style>
</head>
<body>
<header><div class="brand">Water Quality Monitor</div><div class="ip" id="ip"></div></header>
<div class="wrap">
  <div class="grid">
    <div class="card"><div class="label">pH</div><div class="value"><span id="ph">--</span><span class="unit"></span></div><div id="phStatus" class="status">--</div></div>
    <div class="card"><div class="label">Turbidity</div><div class="value"><span id="ntu">--</span><span class="unit"> NTU</span></div><div id="ntuStatus" class="status">--</div></div>
    <div class="card"><div class="label">TDS</div><div class="value"><span id="tds">--</span><span class="unit"> ppm</span></div><div id="tdsStatus" class="status">--</div></div>
    <div class="card"><div class="label">Heading</div><div class="compassWrap"><div class="compass"><div id="needle" class="needle"></div></div><div><div class="value"><span id="heading">--</span><span class="unit">°</span></div><div id="dir" class="status">--</div></div></div></div>
  </div>
  <div class="row" style="margin-top:14px;">
    <div class="card"><div class="label">GPS</div><div class="value" style="font-size:1.2rem"><span id="lat">--</span>, <span id="lng">--</span></div><div id="map"></div>
      <div class="btnbar">
        <button class="btn" onclick="downloadCSV()">Download CSV</button>
        <button class="btn" onclick="controlRelay('on')">Turn OFF</button>
        <button class="btn red" onclick="controlRelay('off')">Turn ON Relay</button>
      </div>
    </div>
    <div class="card">
      <div class="label">Live Charts (last ~2 min)</div>
      <div class="charts">
        <canvas id="chartPH" height="140"></canvas>
        <canvas id="chartTDS" height="140"></canvas>
        <canvas id="chartNTU" height="140"></canvas>
      </div>
    </div>
  </div>
</div>
<footer>Built for UNO R4 WiFi • Leaflet + Chart.js on the client</footer>
<script>
  const map = L.map('map').setView([0,0], 2);
  L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', { maxZoom: 18 }).addTo(map);
  const marker = L.marker([0,0]).addTo(map);
  const labels = [], dataPH = [], dataTDS = [], dataNTU = [];
  function mkChart(ctx, ylabel, borderColor){
    return new Chart(ctx, {
      type: 'line',
      data: { labels, datasets: [{ label: ylabel, data: [], borderColor, tension: .25, pointRadius: 0 }]},
      options: { responsive:true, animation:false, plugins:{ legend:{ display:false }}, scales:{ x:{ display:false }, y:{ ticks:{ color:'#cbd5e1' }, grid:{ color:'#1f2937' } } } }
    });
  }
  const chPH  = mkChart(document.getElementById('chartPH'), 'pH', '#06b6d4');
  const chTDS = mkChart(document.getElementById('chartTDS'), 'TDS (ppm)', '#10b981');
  const chNTU = mkChart(document.getElementById('chartNTU'), 'Turbidity (NTU)', '#f59e0b');
  function statusBadge(el, value, goodRange, warnRange){
    el.className = 'status';
    if (value>=goodRange[0] && value<=goodRange[1]) { el.classList.add('good'); el.textContent='Good'; }
    else if (value>=warnRange[0] && value<=warnRange[1]) { el.classList.add('warn'); el.textContent='Watch'; }
    else { el.classList.add('bad'); el.textContent='Alert'; }
  }
  function dirFromHeading(h){
    const dirs = ['N','NE','E','SE','S','SW','W','NW'];
    return dirs[Math.round(h/45)%8];
  }
  function downloadCSV(){ window.location.href='/download'; }
  function controlRelay(action) {
    fetch('/relay_' + action).then(r => console.log('Relay action sent:', action));
  }
  async function refresh(){
    try{
      const r = await fetch('/data'); if(!r.ok) return;
      const d = await r.json();
      document.getElementById('ip').textContent = d.ip || '';
      document.getElementById('ph').textContent  = d.ph.toFixed(2);
      document.getElementById('ntu').textContent = d.ntu.toFixed(1);
      document.getElementById('tds').textContent = d.tds.toFixed(0);
      document.getElementById('heading').textContent = d.heading.toFixed(1);
      document.getElementById('lat').textContent = d.hasFix ? d.lat.toFixed(6) : '--';
      document.getElementById('lng').textContent = d.hasFix ? d.lng.toFixed(6) : '--';
      statusBadge(document.getElementById('phStatus'), d.ph, [6.5,8.5], [6.0,9.0]);
      statusBadge(document.getElementById('ntuStatus'), d.ntu, [0,5], [5,20]);
      statusBadge(document.getElementById('tdsStatus'), d.tds, [0,500], [500,700]);
      document.getElementById('needle').style.transform = `translate(-50%,-100%) rotate(${d.heading}deg)`;
      const dir = dirFromHeading(d.heading);
      const dirEl = document.getElementById('dir');
      dirEl.className = 'status';
      dirEl.textContent = dir;
      if(d.hasFix){
        marker.setLatLng([d.lat,d.lng]);
        if(!refresh._inited){ map.setView([d.lat,d.lng], 15); refresh._inited=true; }
      }
      const ts = (d.ms/1000).toFixed(0);
      labels.push(ts); dataPH.push(d.ph); dataTDS.push(d.tds); dataNTU.push(d.ntu);
      if(labels.length>60){ labels.shift(); dataPH.shift(); dataTDS.shift(); dataNTU.shift(); }
      chPH.data.datasets[0].data = dataPH;
      chTDS.data.datasets[0].data = dataTDS;
      chNTU.data.datasets[0].data = dataNTU;
      chPH.update(); chTDS.update(); chNTU.update();
    }catch(e){ console.error(e); }
  }
  setInterval(refresh, 2000); refresh();
</script>
</body>
</html>
)rawliteral";

// -----------------------

float readVoltage(int pin) {
  int raw = analogRead(pin);
  return raw * (VREF / ADC_MAX);
}

float turbidityFromVoltage(float v) {
  float ntu = -1120.4f * v * v + 5742.3f * v - 4352.9f;
  return (ntu < 0) ? 0 : ntu;
}

float tdsFromVoltage(float v) {
  float compCoef = 1.0f;
  float vComp = v / compCoef;
  float ec = 133.42f * pow(vComp, 3) - 255.86f * pow(vComp, 2) + 857.39f * vComp;
  float tds = ec * 0.5f;
  return (tds < 0) ? 0 : tds;
}

void pushLog(float ph, float ntu, float tds, float heading, bool hasFix, double lat, double lng) {
  LogEntry e;
  e.ms = millis();
  e.ph = ph; e.ntu = ntu; e.tds = tds;
  e.heading = heading;
  e.hasFix = hasFix; e.lat = lat; e.lng = lng;
  logBuf[logHead] = e;
  logHead = (logHead + 1) % LOG_CAP;
  if (logCount < LOG_CAP) logCount++;
}

void sendIndex(WiFiClient& client) {
  client.println(F("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n"));
  client.print(INDEX_HTML);
}

void sendData(WiFiClient& client, const LogEntry& e) {
  client.println(F("HTTP/1.1 200 OK"));
  client.println(F("Content-Type: application/json"));
  client.println(F("Cache-Control: no-store"));
  client.println();
  client.print(F("{"));
  client.print(F("\"ms\":")); client.print(e.ms);
  client.print(F(",\"ph\":")); client.print(e.ph, 3);
  client.print(F(",\"ntu\":")); client.print(e.ntu, 3);
  client.print(F(",\"tds\":")); client.print(e.tds, 3);
  client.print(F(",\"heading\":")); client.print(e.heading, 2);
  client.print(F(",\"hasFix\":")); client.print(e.hasFix ? F("true"):F("false"));
  client.print(F(",\"lat\":")); client.print(e.lat, 6);
  client.print(F(",\"lng\":")); client.print(e.lng, 6);
  client.print(F(",\"ip\":\""));
  client.print(WiFi.localIP());
  client.print(F("\"}"));
}

void sendCSV(WiFiClient& client) {
  client.println(F("HTTP/1.1 200 OK"));
  client.println(F("Content-Type: text/csv"));
  client.println(F("Content-Disposition: attachment; filename=water_log.csv"));
  client.println();
  client.println(F("Time(ms),pH,Turbidity(NTU),TDS(ppm),Heading(deg),Latitude,Longitude"));
  for (int i = 0; i < logCount; i++) {
    int idx = (logHead - logCount + i + LOG_CAP) % LOG_CAP;
    const LogEntry &e = logBuf[idx];
    client.print(e.ms); client.print(',');
    client.print(e.ph, 3); client.print(',');
    client.print(e.ntu, 3); client.print(',');
    client.print(e.tds, 3); client.print(',');
    client.print(e.heading, 2); client.print(',');
    if (e.hasFix) { client.print(e.lat, 6); client.print(','); client.print(e.lng, 6); }
    else { client.print(','); client.print(','); }
    client.println();
  }
}

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600);
  Wire.begin();
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  analogReadResolution(12);
  compass.init();
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts++ < 60) {
    delay(250);
  }
  server.begin();
  Serial.print(F("IP: "));
  Serial.println(WiFi.localIP());
}
void loop() {
  while (Serial1.available()) gps.encode(Serial1.read());
  unsigned long now = millis();
  static float ph=0, ntu=0, tds=0, heading=0;
  static double lat=0, lng=0; static bool hasFix=false;
  if (now - lastSampleMs >= SAMPLE_PERIOD_MS) {
    lastSampleMs = now;
    float vph   = readVoltage(PH_PIN);
    float vturb = readVoltage(TURB_PIN);
    float vtds  = readVoltage(TDS_PIN);
    ph  = 7.0f + (vph - PH_V_NEUTRAL) * PH_SLOPE;
    ntu = turbidityFromVoltage(vturb);
    tds = tdsFromVoltage(vtds);
    compass.read();
    heading = compass.getAzimuth();
    hasFix = gps.location.isValid();
    if (hasFix) { lat = gps.location.lat(); lng = gps.location.lng(); }
  }
  if (now - lastLogMs >= LOG_PERIOD_MS) {
    lastLogMs = now;
    pushLog(ph, ntu, tds, heading, hasFix, lat, lng);
  }
  WiFiClient client = server.available();
  if (client) {
    String req = client.readStringUntil('\r');
    client.flush();
    if (req.indexOf(F("GET /relay_on")) >= 0) {
      relayState = true;
      digitalWrite(RELAY_PIN, HIGH);
      client.println(F("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nRelay ON"));
    } else if (req.indexOf(F("GET /relay_off")) >= 0) {
      relayState = false;
      digitalWrite(RELAY_PIN, LOW);
      client.println(F("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nRelay OFF"));
    } else if (req.startsWith(F("GET / "))) {
      sendIndex(client);
    } else if (req.indexOf(F("GET /data")) >= 0) {
      if (logCount > 0) {
        int idx = (logHead - 1 + LOG_CAP) % LOG_CAP;
        sendData(client, logBuf[idx]);
      } else {
        LogEntry e{}; e.ms=0; e.ph=0; e.ntu=0; e.tds=0; e.heading=0; e.hasFix=false; e.lat=0; e.lng=0;
        sendData(client, e);
      }
    } else if (req.indexOf(F("GET /download")) >= 0) {
      sendCSV(client);
    } else {
      sendIndex(client);
    }
    client.stop();
  }
}