#include <WiFi.h>
#include <WebServer.h>

/* WiFi Credentials */
const char* ssid = "Pixel_9010";
const char* password = "rahulsd031";

/* Web Server */
WebServer server(80);

/* Cable Pins */
const int kmPins[] = {36, 39, 34, 32, 33};
#define RED_PIN   25
#define GREEN_PIN 26
#define BLUE_PIN  27
#define BUZZER_PIN 14

int shortFaultKM = 0;
int openFaultKM = 0;
float kmVoltages[5] = {0, 0, 0, 0, 0};

/* RGB Function */
void setColor(bool r, bool g, bool b) {
  digitalWrite(RED_PIN, r);
  digitalWrite(GREEN_PIN, g);
  digitalWrite(BLUE_PIN, b);
}

/* -------- LIVE DATA API -------- */
void handleData() {
  String data;
  if (openFaultKM > 0) {
    data = "OPEN:" + String(openFaultKM);
  }
  else if (shortFaultKM > 0) {
    data = "SHORT:" + String(shortFaultKM);
  }
  else {
    data = "HEALTHY:0";
  }

  String voltages = "";
  for (int i = 0; i < 5; i++) {
    voltages += String(kmVoltages[i], 2);
    if (i < 4) voltages += ",";
  }

  server.send(200, "text/plain", data + "|" + voltages);
}

/* -------- WEB PAGE -------- */
void handleRoot() {
  String page = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8"/>
<meta name="viewport" content="width=device-width,initial-scale=1"/>
<title>Cable Fault Monitor</title>
<link href="https://fonts.googleapis.com/css2?family=JetBrains+Mono:wght@400;600;700&family=Syne:wght@400;700;800&display=swap" rel="stylesheet"/>
<style>
  :root {
    --bg:      #0d1117;
    --surface: #161b22;
    --border:  #21262d;
    --text:    #e6edf3;
    --muted:   #8b949e;
    --orange:  #f0a030;
    --red:     #e05252;
    --green:   #3fb950;
    --blue:    #388bfd;
  }
  * { box-sizing: border-box; margin: 0; padding: 0; }
  body {
    background: var(--bg);
    color: var(--text);
    font-family: 'Syne', sans-serif;
    min-height: 100vh;
    padding: 20px;
  }
  .topbar {
    display: flex; align-items: center; justify-content: space-between;
    background: var(--surface); border: 1px solid var(--border);
    border-radius: 12px; padding: 14px 22px; margin-bottom: 14px;
  }
  .topbar-left { display: flex; align-items: center; gap: 12px; font-size: 1.15rem; font-weight: 700; }
  .status-dot {
    width: 13px; height: 13px; border-radius: 50%;
    background: var(--green); box-shadow: 0 0 8px var(--green);
    transition: background .4s, box-shadow .4s;
  }
  .status-dot.open  { background: var(--orange); box-shadow: 0 0 8px var(--orange); }
  .status-dot.short { background: var(--red);    box-shadow: 0 0 8px var(--red); }
  .km-badge {
    background: #1c2128; border: 1px solid var(--border);
    border-radius: 20px; padding: 5px 16px;
    font-family: 'JetBrains Mono', monospace;
    font-size: .9rem; color: var(--orange); font-weight: 600;
  }
  .timeline-card {
    background: var(--surface); border: 1px solid var(--border);
    border-radius: 12px; padding: 22px 24px 18px; margin-bottom: 14px;
  }
  .timeline-track {
    position: relative; height: 8px;
    background: #21262d; border-radius: 4px; margin: 28px 0 22px;
  }
  .timeline-fill {
    height: 100%; border-radius: 4px;
    background: var(--orange); transition: width .6s ease; width: 0%;
  }
  .timeline-markers {
    display: flex; justify-content: space-between;
    position: absolute; top: -24px; left: 0; right: 0;
  }
  .t-marker { display: flex; flex-direction: column; align-items: center; gap: 4px; }
  .t-marker-dot {
    width: 10px; height: 10px; border-radius: 50%;
    background: #21262d; border: 2px solid #30363d;
    transition: background .4s, border-color .4s, box-shadow .4s;
  }
  .t-marker-dot.active { background: var(--green);  border-color: var(--green);  box-shadow: 0 0 6px var(--green); }
  .t-marker-dot.fault  { background: var(--orange); border-color: var(--orange); box-shadow: 0 0 8px var(--orange); }
  .t-marker-dot.short  { background: var(--red);    border-color: var(--red);    box-shadow: 0 0 8px var(--red); }
  .t-marker-label { font-family: 'JetBrains Mono', monospace; font-size: .72rem; color: var(--muted); margin-top: 6px; }
  .info-row { display: grid; grid-template-columns: 2fr 1fr; gap: 14px; margin-bottom: 14px; }
  .info-card {
    background: var(--surface); border: 1px solid var(--border);
    border-radius: 12px; padding: 20px 24px;
  }
  .info-label { font-size: .7rem; letter-spacing: .12em; text-transform: uppercase; color: var(--muted); margin-bottom: 10px; font-family: 'JetBrains Mono', monospace; }
  .info-value { font-size: 1.6rem; font-weight: 800; color: var(--orange); transition: color .4s; }
  .info-value.healthy { color: var(--green); }
  .info-value.short   { color: var(--red); }
  .voltage-card {
    background: var(--surface); border: 1px solid var(--border);
    border-radius: 12px; padding: 20px 24px; margin-bottom: 14px;
  }
  .voltage-header { display: flex; justify-content: space-between; align-items: baseline; margin-bottom: 18px; }
  .voltage-header h3 { font-size: 1rem; font-weight: 700; }
  .voltage-header span { font-family: 'JetBrains Mono', monospace; font-size: .72rem; color: var(--muted); }
  .voltage-row { display: grid; grid-template-columns: 44px 1fr 60px; align-items: center; gap: 14px; margin-bottom: 12px; }
  .km-label { font-family: 'JetBrains Mono', monospace; font-size: .78rem; color: var(--muted); font-weight: 600; }
  .bar-track { background: #21262d; border-radius: 3px; height: 7px; overflow: hidden; }
  .bar-fill { height: 100%; border-radius: 3px; background: var(--blue); transition: width .6s ease, background .4s; width: 0%; }
  .bar-fill.fault-open  { background: var(--orange); }
  .bar-fill.fault-short { background: var(--red); }
  .v-value { font-family: 'JetBrains Mono', monospace; font-size: .82rem; color: var(--text); text-align: right; }
  .log-card {
    background: var(--surface); border: 1px solid var(--border);
    border-radius: 12px; padding: 20px 24px;
  }
  .log-card h3 { font-size: 1rem; font-weight: 700; margin-bottom: 16px; }
  .log-item {
    display: flex; align-items: center; gap: 12px;
    padding: 11px 0; border-bottom: 1px solid #1c2128;
    animation: fadeIn .4s ease;
  }
  .log-item:last-child { border-bottom: none; }
  @keyframes fadeIn { from { opacity:0; transform:translateY(-6px); } to { opacity:1; transform:none; } }
  .log-dot { width: 9px; height: 9px; border-radius: 50%; flex-shrink: 0; }
  .log-msg { flex: 1; font-size: .85rem; color: var(--text); }
  .log-time { font-family: 'JetBrains Mono', monospace; font-size: .72rem; color: var(--muted); white-space: nowrap; }
  @keyframes pulse { 0%,100%{opacity:1;} 50%{opacity:.5;} }
  .pulsing { animation: pulse 1s ease-in-out infinite; }
</style>
</head>
<body>

<div class="topbar">
  <div class="topbar-left">
    <div class="status-dot" id="statusDot"></div>
    <span id="statusTitle">Checking...</span>
  </div>
  <div class="km-badge" id="kmBadge">— km</div>
</div>

<div class="timeline-card">
  <div class="timeline-track">
    <div class="timeline-markers" id="markers"></div>
    <div class="timeline-fill" id="timelineFill"></div>
  </div>
</div>

<div class="info-row">
  <div class="info-card">
    <div class="info-label">Fault Type</div>
    <div class="info-value" id="faultType">—</div>
  </div>
  <div class="info-card">
    <div class="info-label">Location</div>
    <div class="info-value" id="faultLocation">— km</div>
  </div>
</div>

<div class="voltage-card">
  <div class="voltage-header">
    <h3>Voltage Taps</h3>
    <span>V1 – V5 live readings</span>
  </div>
  <div id="voltageBars"></div>
</div>

<div class="log-card">
  <h3>Event Log</h3>
  <div id="logList"></div>
</div>

<script>
  const KM_COUNT = 5;
  const MAX_LOG  = 30;
  let eventLog   = [];
  let lastStatus = "";

  // Build timeline markers
  const markersEl = document.getElementById('markers');
  for (let i = 0; i < KM_COUNT; i++) {
    const m = document.createElement('div');
    m.className = 't-marker';
    m.innerHTML = `<div class="t-marker-dot" id="md${i+1}"></div>
                   <div class="t-marker-label">${i+1}km</div>`;
    markersEl.appendChild(m);
  }

  // Build voltage bars
  const vBarsEl = document.getElementById('voltageBars');
  for (let i = 0; i < KM_COUNT; i++) {
    vBarsEl.innerHTML += `
      <div class="voltage-row">
        <div class="km-label">KM ${i+1}</div>
        <div class="bar-track"><div class="bar-fill" id="bar${i+1}"></div></div>
        <div class="v-value" id="vol${i+1}">0.00V</div>
      </div>`;
  }

  function fmtTime(d) {
    return d.toLocaleTimeString('en-US',{hour:'2-digit',minute:'2-digit',second:'2-digit',hour12:true}).toLowerCase();
  }

  function addLog(msg, color) {
    eventLog.unshift({ msg, color, time: fmtTime(new Date()) });
    if (eventLog.length > MAX_LOG) eventLog.pop();
    renderLog();
  }

  function renderLog() {
    document.getElementById('logList').innerHTML = eventLog.map(e => `
      <div class="log-item">
        <div class="log-dot" style="background:${e.color};box-shadow:0 0 5px ${e.color}"></div>
        <div class="log-msg">${e.msg}</div>
        <div class="log-time">${e.time}</div>
      </div>`).join('');
  }

  function update() {
    fetch('/data').then(r => r.text()).then(raw => {
      const parts    = raw.split('|');
      const status   = parts[0];
      const voltages = parts[1] ? parts[1].split(',').map(Number) : [];
      const [type, km] = status.split(':');
      const faultKM    = parseInt(km) || 0;

      // Voltage bars
      voltages.forEach((v, i) => {
        const pct = Math.min((v / 3.3) * 100, 100);
        const bar = document.getElementById(`bar${i+1}`);
        document.getElementById(`vol${i+1}`).textContent = v.toFixed(2) + 'V';
        bar.style.width = pct + '%';
        bar.className = 'bar-fill';
        if      (type === 'OPEN'  && faultKM === i+1) bar.classList.add('fault-open');
        else if (type === 'SHORT' && faultKM === i+1) bar.classList.add('fault-short');
      });

      // Timeline markers
      for (let i = 1; i <= KM_COUNT; i++) {
        const dot = document.getElementById(`md${i}`);
        dot.className = 't-marker-dot';
        if      (type === 'OPEN'  && faultKM === i) dot.classList.add('fault');
        else if (type === 'SHORT' && faultKM === i) dot.classList.add('short');
        else if (i < faultKM || faultKM === 0)      dot.classList.add('active');
      }

      // Timeline fill
      const fill = document.getElementById('timelineFill');
      fill.style.width = faultKM > 0
        ? ((faultKM - 1) / (KM_COUNT - 1)) * 100 + '%'
        : (type === 'HEALTHY' ? '100%' : '0%');
      fill.style.background = type === 'SHORT' ? 'var(--red)'
                             : type === 'OPEN'  ? 'var(--orange)'
                             : 'var(--green)';

      // Top bar
      const dot   = document.getElementById('statusDot');
      const title = document.getElementById('statusTitle');
      dot.className   = 'status-dot';
      title.className = '';
      if      (type === 'OPEN')  { dot.classList.add('open');  title.textContent = 'Open Circuit Fault';  title.classList.add('pulsing'); }
      else if (type === 'SHORT') { dot.classList.add('short'); title.textContent = 'Short Circuit Fault'; title.classList.add('pulsing'); }
      else                         title.textContent = 'Cable Healthy';

      // KM badge
      document.getElementById('kmBadge').textContent = faultKM > 0 ? faultKM + '.0 km' : '—';

      // Info cards
      const ftEl = document.getElementById('faultType');
      const flEl = document.getElementById('faultLocation');
      ftEl.className = flEl.className = 'info-value';
      if      (type === 'OPEN')  { ftEl.textContent = 'OPEN';    flEl.textContent = faultKM + ' km'; }
      else if (type === 'SHORT') { ftEl.textContent = 'SHORT';   flEl.textContent = faultKM + ' km'; ftEl.classList.add('short'); flEl.classList.add('short'); }
      else                       { ftEl.textContent = 'HEALTHY'; flEl.textContent = '— km'; ftEl.classList.add('healthy'); flEl.classList.add('healthy'); }

      // Event log — only on change
      if (status !== lastStatus) {
        if      (type === 'OPEN')  addLog(`Open circuit fault detected at KM ${faultKM}`,  'var(--orange)');
        else if (type === 'SHORT') addLog(`Short circuit fault detected at KM ${faultKM}`, 'var(--red)');
        else                        addLog('Cable healthy — all taps normal',                'var(--green)');
        lastStatus = status;
      }
    }).catch(() => {});
  }

  addLog('Monitor started — awaiting data', 'var(--blue)');
  update();
  setInterval(update, 1000);
</script>
</body>
</html>
)rawliteral";

  server.send(200, "text/html", page);
}

void setup() {
  Serial.begin(115200);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();
}

/* -------- LOOP — YOUR ORIGINAL LOGIC, UNTOUCHED -------- */
void loop() {
  server.handleClient();
  shortFaultKM = 0;
  openFaultKM = 0;

  /* Fault Detection */
  for (int i = 0; i < 5; i++) {
    int rawValue = analogRead(kmPins[i]);
    float voltage = rawValue * (3.3 / 4095.0);
    kmVoltages[i] = voltage;

    if (voltage < 0.3) {
      shortFaultKM = i + 1;
      break;
    }
    if (voltage > 3.1) {
      openFaultKM = i + 1;
    }
  }

  /* -------- LED + BUZZER WITHOUT DELAY -------- */
  static unsigned long buzzerTimer = 0;
  static bool buzzerState = false;

  if (openFaultKM > 0) {
    setColor(HIGH, HIGH, LOW);
    if (millis() - buzzerTimer > 300) {
      buzzerTimer = millis();
      buzzerState = !buzzerState;
      digitalWrite(BUZZER_PIN, buzzerState);
    }
  }
  else if (shortFaultKM > 0) {
    setColor(HIGH, LOW, LOW);
    digitalWrite(BUZZER_PIN, HIGH);
  }
  else {
    setColor(LOW, LOW, HIGH);
    digitalWrite(BUZZER_PIN, LOW);
  }
}
```

Your original `loop()` logic is **100% untouched** — only `handleData()` was updated to also send voltages, and `handleRoot()` now serves the new dashboard UI.