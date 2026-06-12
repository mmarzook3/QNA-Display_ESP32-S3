#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <WebServer.h>
#include <WiFi.h>

const char* WIFI_SSID = "M3_Devices";
const char* WIFI_PASSWORD = "Marzook7494.";

enum DisplayMode {
  MODE_PART,
  MODE_CUSTOM,
  MODE_BLANK,
  MODE_THINKING
};

// Waveshare ESP32-S3-LCD-1.47 pin mapping (from official wiki)
constexpr uint8_t TFT_MOSI = 45;
constexpr uint8_t TFT_SCLK = 40;
constexpr uint8_t TFT_CS   = 42;
constexpr uint8_t TFT_DC   = 41;
constexpr uint8_t TFT_RST  = 39;
constexpr uint8_t TFT_BL   = 48;

constexpr uint16_t PANEL_WIDTH = 320;
constexpr uint16_t PANEL_HEIGHT = 172;

const uint16_t BG_COLOR = ST77XX_BLACK;
const uint16_t INACTIVE_PART_COLOR = 0x7BEF;

Adafruit_ST7789 tft(&SPI, TFT_CS, TFT_DC, TFT_RST);
WebServer server(80);

char currentPart = 'A';
String customText = "";
DisplayMode currentMode = MODE_PART;
uint16_t screenW = PANEL_WIDTH;
uint16_t screenH = PANEL_HEIGHT;
bool lastConnected = false;
bool blinkState = false;
unsigned long lastBlinkToggle = 0;
uint8_t thinkingFrame = 0;
unsigned long lastThinkingTick = 0;
bool questionFlagged = false;

String currentDisplayText() {
  if (currentMode == MODE_CUSTOM) {
    return customText;
  }
  if (currentMode == MODE_THINKING) {
    if (thinkingFrame == 0) return ".";
    if (thinkingFrame == 1) return ". .";
    return ". . .";
  }
  return String(currentPart);
}

String currentModeName() {
  if (currentMode == MODE_PART) return "part";
  if (currentMode == MODE_CUSTOM) return "custom";
  if (currentMode == MODE_BLANK) return "blank";
  return "thinking";
}

void drawThinkingDots() {
  const int16_t baseX = 120;
  const int16_t baseY = 66;
  const int16_t spacing = 28;
  const int16_t radius = 6;

  tft.fillRect(baseX - 12, baseY - 18, 3 * spacing + 24, 36, BG_COLOR);
  tft.setTextSize(2);
  if (WiFi.status() == WL_CONNECTED) {
    tft.setCursor(8, 108);
    tft.setTextColor(ST77XX_WHITE, BG_COLOR);
    tft.print(WiFi.localIP().toString());
  }

  for (uint8_t i = 0; i < 3; i++) {
    int16_t y = baseY;
    if (i == thinkingFrame) {
      y -= 10;
    }
    tft.fillCircle(baseX + i * spacing, y, radius, ST77XX_YELLOW);
  }
}

void drawConnectionDot(bool connected) {
  const int16_t dotX = screenW - 12;
  const int16_t dotY = 12;
  tft.fillCircle(dotX, dotY, 6, ST77XX_BLACK);

  uint16_t dotColor = ST77XX_BLACK;
  if (connected) {
    dotColor = ST77XX_GREEN;
  } else if (blinkState) {
    dotColor = ST77XX_RED;
  }
  tft.fillCircle(dotX, dotY, 6, dotColor);
}

void drawFlagIndicator() {
  if (!questionFlagged || currentMode == MODE_BLANK) {
    return;
  }
  tft.fillRoundRect(8, 8, 58, 20, 4, ST77XX_RED);
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_WHITE, ST77XX_RED);
  tft.setCursor(18, 14);
  tft.print("FLAG");
}

void drawPartList() {
  const char* parts[] = {"A", "B", "C", "D"};
  tft.setTextSize(2);
  const int16_t top = screenH - 22;
  const int16_t segmentWidth = screenW / 4;

  for (uint8_t i = 0; i < 4; i++) {
    const int16_t x = i * segmentWidth + ((segmentWidth - 12) / 2);
    uint16_t color = (currentMode == MODE_PART && parts[i][0] == currentPart) ? ST77XX_WHITE : INACTIVE_PART_COLOR;
    tft.setTextColor(color, BG_COLOR);
    tft.setCursor(x, top);
    tft.print(parts[i]);
  }
}

void renderScreen() {
  bool connected = (WiFi.status() == WL_CONNECTED);
  tft.fillScreen(BG_COLOR);

  if (currentMode == MODE_BLANK) {
    return;
  }

  drawConnectionDot(connected);
  drawFlagIndicator();
  if (currentMode == MODE_THINKING) {
    drawThinkingDots();
    drawPartList();
    return;
  }

  tft.setTextSize(5);
  tft.setTextColor(ST77XX_YELLOW, BG_COLOR);
  tft.setCursor(18, 28);
  tft.print(currentDisplayText());

  tft.setTextSize(2);
  if (connected) {
    tft.setCursor(8, 108);
    tft.print(WiFi.localIP().toString());
  }

  drawPartList();
}

void handleRoot() {
  String html =
      "<!doctype html><html><head><meta name='viewport' content='width=device-width, initial-scale=1'>"
      "<style>body{font-family:Arial,sans-serif;padding:20px;background:#f3f3f3;color:#111827;}button{padding:14px 22px;font-size:20px;border:none;color:white;background:#2563eb;border-radius:10px;width:120px;cursor:pointer;}button.active{background:#16a34a;}button.busy{background:#f59e0b;}button.flagged{background:#dc2626;}button:disabled,input:disabled{opacity:.7;cursor:wait;}input{padding:12px 14px;font-size:18px;border:1px solid #cbd5e1;border-radius:10px;width:252px;box-sizing:border-box;}.grid{display:grid;grid-template-columns:1fr 1fr;gap:12px;max-width:280px;}#status{margin-top:16px;font-weight:700;}#wifi{margin-top:8px;color:#374151;}#customRow{margin-top:14px;display:flex;gap:12px;align-items:center;flex-wrap:wrap;}#customBtn{background:#7c3aed;}#clearBtn{background:#6b7280;}#flagBtn{background:#b91c1c;}</style>"
      "</head><body><h2>ESP32-S3 Interview Controller</h2>"
      "<div class='grid'>"
      "<button id='btnA' onclick=\"selectPart('A')\">PART A</button>"
      "<button id='btnB' onclick=\"selectPart('B')\">PART B</button>"
      "<button id='btnC' onclick=\"selectPart('C')\">PART C</button>"
      "<button id='btnD' onclick=\"selectPart('D')\">PART D</button>"
      "<button id='btnBlank' onclick=\"setMode('blank')\">BLANK</button>"
      "<button id='btnThinking' onclick=\"setMode('thinking')\">. . .</button>"
      "<button id='flagBtn' onclick=\"toggleFlag()\">FLAG</button>"
      "</div>"
      "<div id='customRow'><input id='customText' maxlength='12' placeholder='Custom text'><button id='customBtn' onclick=\"sendCustom()\">SHOW TEXT</button><button id='clearBtn' onclick=\"clearCustom()\">CLEAR</button></div>"
      "<p id='status'>Current: <b>" + currentDisplayText() + "</b></p>"
      "<p id='wifi'>Loading status...</p>"
      "<script>"
      "let currentState={mode:'" + currentModeName() + "',part:'" + String(currentPart) + "',text:'" + customText + "',display:'" + currentDisplayText() + "',flagged:" + String(questionFlagged ? "true" : "false") + "};"
      "function setBusy(pending){['btnA','btnB','btnC','btnD','btnBlank','btnThinking','flagBtn','customBtn','clearBtn','customText'].forEach(id=>{document.getElementById(id).disabled=!!pending;});}"
      "function paintButtons(state,pending){['btnA','btnB','btnC','btnD','btnBlank','btnThinking','flagBtn','customBtn','clearBtn'].forEach(id=>document.getElementById(id).classList.remove('active','busy','flagged'));const activeClass=pending?'busy':'active';if(state.mode==='part'){document.getElementById('btn'+state.part).classList.add(activeClass);}else if(state.mode==='blank'){document.getElementById('btnBlank').classList.add(activeClass);}else if(state.mode==='thinking'){document.getElementById('btnThinking').classList.add(activeClass);}else if(state.mode==='custom'){document.getElementById('customBtn').classList.add(activeClass);}if(state.flagged){document.getElementById('flagBtn').classList.add(pending?'busy':'flagged');}document.getElementById('status').innerHTML='Current: <b>'+state.display+'</b>'+(state.flagged?' | <span style=\"color:#dc2626\">Flagged</span>':'');const input=document.getElementById('customText');const typing=document.activeElement===input&&!pending;if(!typing){input.value=state.text||'';}setBusy(pending);}"
      "async function refreshState(){try{const r=await fetch('/state?ts='+Date.now(),{cache:'no-store'});const d=await r.json();currentState=d;paintButtons(currentState,false);document.getElementById('wifi').innerText=d.wifi?('IP: '+d.ip):'Connecting...';}catch(e){document.getElementById('wifi').innerText='Controller unreachable';}}"
      "async function selectPart(part){const next={...currentState,mode:'part',part:part,display:part};paintButtons(next,true);try{const r=await fetch('/set?part='+part+'&ts='+Date.now(),{cache:'no-store'});if(!r.ok)throw new Error('set failed');await refreshState();}catch(e){paintButtons(currentState,false);document.getElementById('wifi').innerText='Update failed';}}"
      "async function setMode(mode){const next={...currentState,mode:mode,display:(mode==='blank'?'BLANK':'. . .')};paintButtons(next,true);try{const r=await fetch('/mode?value='+encodeURIComponent(mode)+'&ts='+Date.now(),{cache:'no-store'});if(!r.ok)throw new Error('mode failed');await refreshState();}catch(e){paintButtons(currentState,false);document.getElementById('wifi').innerText='Update failed';}}"
      "async function toggleFlag(){const next={...currentState,flagged:!currentState.flagged};paintButtons(next,true);try{const r=await fetch('/flag?value='+(currentState.flagged?'off':'on')+'&ts='+Date.now(),{cache:'no-store'});if(!r.ok)throw new Error('flag failed');await refreshState();}catch(e){paintButtons(currentState,false);document.getElementById('wifi').innerText='Flag update failed';}}"
      "async function sendCustom(){const value=document.getElementById('customText').value.trim();if(!value)return;const next={...currentState,mode:'custom',text:value,display:value};paintButtons(next,true);try{const r=await fetch('/custom?text='+encodeURIComponent(value)+'&ts='+Date.now(),{cache:'no-store'});if(!r.ok)throw new Error('custom failed');await refreshState();}catch(e){paintButtons(currentState,false);document.getElementById('wifi').innerText='Update failed';}}"
      "async function clearCustom(){const next={...currentState,text:''};paintButtons(next,true);try{const r=await fetch('/clear?ts='+Date.now(),{cache:'no-store'});if(!r.ok)throw new Error('clear failed');await refreshState();}catch(e){paintButtons(currentState,false);document.getElementById('wifi').innerText='Clear failed';}}"
      "paintButtons(currentState,false);refreshState();setInterval(refreshState,1200);"
      "</script>"
      "</body></html>";
  server.send(200, "text/html", html);
}

void handleSet() {
  if (!server.hasArg("part")) {
    server.send(400, "text/plain", "Missing part");
    return;
  }

  const String partArg = server.arg("part");
  if (partArg.length() == 0) {
    server.send(400, "text/plain", "Invalid part");
    return;
  }

  char p = toupper(partArg.charAt(0));
  if (p == 'A' || p == 'B' || p == 'C' || p == 'D') {
    questionFlagged = false;
    currentPart = p;
    currentMode = MODE_PART;
    renderScreen();
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Invalid part");
  }
}

void handleMode() {
  if (!server.hasArg("value")) {
    server.send(400, "text/plain", "Missing mode");
    return;
  }

  const String value = server.arg("value");
  if (value == "blank") {
    questionFlagged = false;
    currentMode = MODE_BLANK;
  } else if (value == "thinking") {
    questionFlagged = false;
    currentMode = MODE_THINKING;
    thinkingFrame = 0;
    lastThinkingTick = millis();
  } else {
    server.send(400, "text/plain", "Invalid mode");
    return;
  }

  renderScreen();
  server.send(200, "text/plain", "OK");
}

void handleCustom() {
  if (!server.hasArg("text")) {
    server.send(400, "text/plain", "Missing text");
    return;
  }

  String value = server.arg("text");
  value.trim();
  if (value.length() == 0) {
    server.send(400, "text/plain", "Empty text");
    return;
  }
  if (value.length() > 12) {
    value = value.substring(0, 12);
  }

  questionFlagged = false;
  customText = value;
  currentMode = MODE_CUSTOM;
  renderScreen();
  server.send(200, "text/plain", "OK");
}

void handleClear() {
  questionFlagged = false;
  customText = "";
  if (currentMode == MODE_CUSTOM) {
    currentMode = MODE_PART;
  }
  renderScreen();
  server.send(200, "text/plain", "OK");
}

void handleFlag() {
  if (!server.hasArg("value")) {
    server.send(400, "text/plain", "Missing flag value");
    return;
  }

  const String value = server.arg("value");
  if (value == "on") {
    questionFlagged = true;
  } else if (value == "off") {
    questionFlagged = false;
  } else {
    server.send(400, "text/plain", "Invalid flag value");
    return;
  }

  renderScreen();
  server.send(200, "text/plain", "OK");
}

void handleState() {
  bool connected = (WiFi.status() == WL_CONNECTED);
  String json = "{";
  json += "\"part\":\"" + String(currentPart) + "\",";
  json += "\"mode\":\"" + currentModeName() + "\",";
  json += "\"text\":\"" + customText + "\",";
  json += "\"display\":\"" + currentDisplayText() + "\",";
  json += "\"flagged\":" + String(questionFlagged ? "true" : "false") + ",";
  json += "\"wifi\":" + String(connected ? "true" : "false") + ",";
  json += "\"ip\":\"" + (connected ? WiFi.localIP().toString() : String("N/A")) + "\"}";
  server.send(200, "application/json", json);
}

bool connectWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  const unsigned long start = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - start < 25000) {
    delay(300);
  }

  return WiFi.status() == WL_CONNECTED;
}

void setup() {
  Serial.begin(115200);
  delay(200);

  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);
  tft.init(PANEL_HEIGHT, PANEL_WIDTH);
  tft.setRotation(3);
  screenW = tft.width();
  screenH = tft.height();
  renderScreen();

  connectWifi();
  renderScreen();

  server.on("/", handleRoot);
  server.on("/set", handleSet);
  server.on("/mode", handleMode);
  server.on("/flag", handleFlag);
  server.on("/custom", handleCustom);
  server.on("/clear", handleClear);
  server.on("/state", handleState);
  server.begin();
}

void loop() {
  server.handleClient();

  bool connected = (WiFi.status() == WL_CONNECTED);
  unsigned long now = millis();
  if (!connected && now - lastBlinkToggle >= 400) {
    blinkState = !blinkState;
    lastBlinkToggle = now;
    if (currentMode != MODE_BLANK) {
      drawConnectionDot(false);
    }
  }
  if (currentMode == MODE_THINKING && now - lastThinkingTick >= 500) {
    thinkingFrame = (thinkingFrame + 1) % 3;
    lastThinkingTick = now;
    drawThinkingDots();
  }
  if (connected != lastConnected) {
    renderScreen();
    lastConnected = connected;
  }
}
