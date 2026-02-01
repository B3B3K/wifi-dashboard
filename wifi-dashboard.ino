#include <SPI.h>
#include <WiFi.h>

// --- TFT Pins ---
#define TFT_CS    0
#define TFT_RST   2
#define TFT_DC    1
#define TFT_MOSI  21
#define TFT_SCLK  20

#define W 160
#define H 80
#define X_OFFSET 1
#define Y_OFFSET 26

// --- ST7735 Commands ---
#define ST7735_SWRESET 0x01
#define ST7735_SLPOUT  0x11
#define ST7735_INVON   0x21
#define ST7735_MADCTL  0x36
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_DISPON  0x29

// --- Colors ---
#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F
#define YELLOW  0xFFE0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define ORANGE  0xFD20
#define GRAY    0x8410

// Signal strength colors
#define COLOR_EXCELLENT 0x07E0  // Green
#define COLOR_GOOD      0x87E0  // Light Green
#define COLOR_FAIR      0xFFE0  // Yellow
#define COLOR_WEAK      0xFD20  // Orange
#define COLOR_POOR      0xF800  // Red

// Graph colors (rainbow effect for different networks)
const uint16_t GRAPH_COLORS[] = {
  0xF800,  // Red
  0xFD20,  // Orange
  0xFFE0,  // Yellow
  0x07E0,  // Green
  0x07FF,  // Cyan
  0x001F,  // Blue
  0xF81F,  // Magenta
  0xFC00   // Pink
};

#define MAX_NETWORKS 8
#define GRAPH_POINTS 26  // 26 points for 80 pixels width (3 pixels per point)
#define SCAN_INTERVAL 1000  // 1 second - faster scanning
#define MAX_SSID_CHARS 6  // Limit SSID display to 6 characters
#define NETWORKS_TO_GRAPH 4  // Track top 4 networks in graph

// --- Complete 5x7 ASCII Font ---
const uint8_t font5x7[][5] = {
  {0x00, 0x00, 0x00, 0x00, 0x00}, // space (32)
  {0x00, 0x00, 0x5F, 0x00, 0x00}, // !
  {0x00, 0x07, 0x00, 0x07, 0x00}, // "
  {0x14, 0x7F, 0x14, 0x7F, 0x14}, // #
  {0x24, 0x2A, 0x7F, 0x2A, 0x12}, // $
  {0x23, 0x13, 0x08, 0x64, 0x62}, // %
  {0x36, 0x49, 0x55, 0x22, 0x50}, // &
  {0x00, 0x05, 0x03, 0x00, 0x00}, // '
  {0x00, 0x1C, 0x22, 0x41, 0x00}, // (
  {0x00, 0x41, 0x22, 0x1C, 0x00}, // )
  {0x08, 0x2A, 0x1C, 0x2A, 0x08}, // *
  {0x08, 0x08, 0x3E, 0x08, 0x08}, // +
  {0x00, 0x50, 0x30, 0x00, 0x00}, // ,
  {0x08, 0x08, 0x08, 0x08, 0x08}, // -
  {0x00, 0x30, 0x30, 0x00, 0x00}, // .
  {0x20, 0x10, 0x08, 0x04, 0x02}, // /
  {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 0 (48)
  {0x00, 0x42, 0x7F, 0x40, 0x00}, // 1
  {0x42, 0x61, 0x51, 0x49, 0x46}, // 2
  {0x21, 0x41, 0x45, 0x4B, 0x31}, // 3
  {0x18, 0x14, 0x12, 0x7F, 0x10}, // 4
  {0x27, 0x45, 0x45, 0x45, 0x39}, // 5
  {0x3C, 0x4A, 0x49, 0x49, 0x30}, // 6
  {0x01, 0x71, 0x09, 0x05, 0x03}, // 7
  {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
  {0x06, 0x49, 0x49, 0x29, 0x1E}, // 9
  {0x00, 0x36, 0x36, 0x00, 0x00}, // :
  {0x00, 0x56, 0x36, 0x00, 0x00}, // ;
  {0x00, 0x08, 0x14, 0x22, 0x41}, // <
  {0x14, 0x14, 0x14, 0x14, 0x14}, // =
  {0x41, 0x22, 0x14, 0x08, 0x00}, // >
  {0x02, 0x01, 0x51, 0x09, 0x06}, // ?
  {0x32, 0x49, 0x79, 0x41, 0x3E}, // @
  {0x7E, 0x11, 0x11, 0x11, 0x7E}, // A (65)
  {0x7F, 0x49, 0x49, 0x49, 0x36}, // B
  {0x3E, 0x41, 0x41, 0x41, 0x22}, // C
  {0x7F, 0x41, 0x41, 0x22, 0x1C}, // D
  {0x7F, 0x49, 0x49, 0x49, 0x41}, // E
  {0x7F, 0x09, 0x09, 0x01, 0x01}, // F
  {0x3E, 0x41, 0x41, 0x49, 0x3A}, // G
  {0x7F, 0x08, 0x08, 0x08, 0x7F}, // H
  {0x00, 0x41, 0x7F, 0x41, 0x00}, // I
  {0x20, 0x40, 0x41, 0x3F, 0x01}, // J
  {0x7F, 0x08, 0x14, 0x22, 0x41}, // K
  {0x7F, 0x40, 0x40, 0x40, 0x40}, // L
  {0x7F, 0x02, 0x04, 0x02, 0x7F}, // M
  {0x7F, 0x04, 0x08, 0x10, 0x7F}, // N
  {0x3E, 0x41, 0x41, 0x41, 0x3E}, // O
  {0x7F, 0x09, 0x09, 0x09, 0x06}, // P
  {0x3E, 0x41, 0x51, 0x21, 0x5E}, // Q
  {0x7F, 0x09, 0x19, 0x29, 0x46}, // R
  {0x46, 0x49, 0x49, 0x49, 0x31}, // S
  {0x01, 0x01, 0x7F, 0x01, 0x01}, // T
  {0x3F, 0x40, 0x40, 0x40, 0x3F}, // U
  {0x1F, 0x20, 0x40, 0x20, 0x1F}, // V
  {0x7F, 0x20, 0x18, 0x20, 0x7F}, // W
  {0x63, 0x14, 0x08, 0x14, 0x63}, // X
  {0x03, 0x04, 0x78, 0x04, 0x03}, // Y
  {0x61, 0x51, 0x49, 0x45, 0x43}  // Z
};

struct WiFiNetwork {
  String ssid;
  int8_t rssi;
  int8_t rssi_history[GRAPH_POINTS];
  uint8_t history_index;
  bool active;
};

WiFiNetwork networks[MAX_NETWORKS];
uint8_t network_count = 0;
unsigned long last_scan = 0;

// --- SPI & Display Functions ---
void writeCmd(uint8_t c) {
  digitalWrite(TFT_DC, LOW);
  digitalWrite(TFT_CS, LOW);
  SPI.transfer(c);
  digitalWrite(TFT_CS, HIGH);
}

void writeData(uint8_t d) {
  digitalWrite(TFT_DC, HIGH);
  digitalWrite(TFT_CS, LOW);
  SPI.transfer(d);
  digitalWrite(TFT_CS, HIGH);
}

void setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
  writeCmd(ST7735_CASET);
  writeData(0);
  writeData(x0 + X_OFFSET);
  writeData(0);
  writeData(x1 + X_OFFSET);
  
  writeCmd(ST7735_RASET);
  writeData(0);
  writeData(y0 + Y_OFFSET);
  writeData(0);
  writeData(y1 + Y_OFFSET);
  
  writeCmd(ST7735_RAMWR);
}

void fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color) {
  if (x >= W || y >= H) return;
  if (x + w > W) w = W - x;
  if (y + h > H) h = H - y;
  
  setAddrWindow(x, y, x + w - 1, y + h - 1);
  uint8_t hi = color >> 8, lo = color & 0xFF;
  
  digitalWrite(TFT_DC, HIGH);
  digitalWrite(TFT_CS, LOW);
  for (uint32_t i = 0; i < (uint32_t)w * h; i++) {
    SPI.transfer(hi);
    SPI.transfer(lo);
  }
  digitalWrite(TFT_CS, HIGH);
}

void drawCharOpaque(uint8_t x, uint8_t y, char c, uint16_t color, uint16_t bg) {
  if (c < 32 || c > 95) return;
  uint8_t idx = c - 32;
  
  setAddrWindow(x, y, x + 4, y + 7);
  digitalWrite(TFT_DC, HIGH);
  digitalWrite(TFT_CS, LOW);
  
  for (int8_t row = 0; row < 8; row++) {
    for (int8_t col = 0; col < 5; col++) {
      uint16_t pColor = (font5x7[idx][col] & (1 << row)) ? color : bg;
      SPI.transfer(pColor >> 8);
      SPI.transfer(pColor & 0xFF);
    }
  }
  digitalWrite(TFT_CS, HIGH);
}

void drawTextOpaque(uint8_t x, uint8_t y, const char* text, uint16_t color, uint16_t bg) {
  while (*text) {
    drawCharOpaque(x, y, *text++, color, bg);
    x += 6;
  }
}

// Get color based on signal strength
uint16_t getSignalColor(int8_t rssi) {
  if (rssi >= -50) return COLOR_EXCELLENT;
  if (rssi >= -60) return COLOR_GOOD;
  if (rssi >= -70) return COLOR_FAIR;
  if (rssi >= -80) return COLOR_WEAK;
  return COLOR_POOR;
}

// Draw WiFi signal strength bars icon
void drawWiFiIcon(uint8_t x, uint8_t y, int8_t rssi, uint16_t color) {
  // 3 bars, height based on signal strength
  uint8_t bars = 0;
  if (rssi >= -80) bars = 1;
  if (rssi >= -70) bars = 2;
  if (rssi >= -60) bars = 3;
  
  for (uint8_t i = 0; i < 3; i++) {
    uint16_t barColor = (i < bars) ? color : GRAY;
    uint8_t barHeight = (i + 1) * 2;
    fillRect(x + i * 3, y + 6 - barHeight, 2, barHeight, barColor);
  }
}

// Draw the WiFi list (left side, 80x80)
void drawWiFiList() {
  fillRect(0, 0, 80, 80, BLACK);
  
  // Title
  drawTextOpaque(2, 1, "WIFI SCAN", WHITE, BLACK);
  fillRect(0, 9, 80, 1, CYAN);
  
  uint8_t y = 12;
  for (uint8_t i = 0; i < network_count && i < 8; i++) {
    if (!networks[i].active) continue;
    
    uint16_t signalColor = getSignalColor(networks[i].rssi);
    uint16_t graphColor = GRAPH_COLORS[i % 8];  // Color matching the graph
    
    // Draw WiFi icon with signal strength color
    drawWiFiIcon(2, y, networks[i].rssi, signalColor);
    
    // Draw SSID (truncated to 6 chars) in graph color
    String ssid = networks[i].ssid;
    if (ssid.length() > MAX_SSID_CHARS) ssid = ssid.substring(0, MAX_SSID_CHARS);
    
    drawTextOpaque(14, y, ssid.c_str(), graphColor, BLACK);  // Use graph color for SSID
    
    // Draw RSSI value in graph color
    char rssi_str[5];
    sprintf(rssi_str, "%d", networks[i].rssi);
    drawTextOpaque(80 - strlen(rssi_str) * 6 - 2, y, rssi_str, graphColor, BLACK);  // Use graph color
    
    y += 9;
  }
  
  // Vertical separator
  fillRect(79, 0, 1, 80, CYAN);
}

// Draw the signal strength graph (right side, 80x80) - shows top 4 networks
void drawGraph() {
  fillRect(80, 0, 80, 80, BLACK);
  
  // Title
  drawTextOpaque(82, 1, "GRAPH", WHITE, BLACK);
  fillRect(80, 9, 80, 1, CYAN);
  
  if (network_count == 0) {
    drawTextOpaque(85, 35, "NO", GRAY, BLACK);
    drawTextOpaque(85, 44, "NETWORK", GRAY, BLACK);
    return;
  }
  
  // Graph area: 80x70 (10 pixels for title)
  uint8_t graph_x = 80;
  uint8_t graph_y = 10;
  uint8_t graph_w = 80;
  uint8_t graph_h = 70;
  
  // Draw grid lines (every 15 dBm from -90 to -30)
  for (int8_t dbm = -90; dbm <= -30; dbm += 15) {
    uint8_t y = graph_y + graph_h - ((dbm + 90) * graph_h / 60);
    for (uint8_t x = 0; x < graph_w; x += 4) {
      fillRect(graph_x + x, y, 2, 1, 0x2104);  // Dark grid
    }
  }
  
  // Draw Y-axis labels
  drawTextOpaque(graph_x + 2, graph_y + 2, "-30", GRAY, BLACK);
  drawTextOpaque(graph_x + 2, graph_y + graph_h - 8, "-90", GRAY, BLACK);
  
  // Draw all active networks (up to 4) with matching colors from left side
  uint8_t networks_drawn = 0;
  for (uint8_t net_idx = 0; net_idx < network_count && networks_drawn < NETWORKS_TO_GRAPH; net_idx++) {
    if (!networks[net_idx].active) continue;
    
    WiFiNetwork& net = networks[net_idx];
    uint16_t lineColor = GRAPH_COLORS[net_idx % 8];  // Same color as left side
    
    // Draw the graph line for this network
    for (uint8_t i = 1; i < GRAPH_POINTS; i++) {
      uint8_t idx1 = (net.history_index + i - 1) % GRAPH_POINTS;
      uint8_t idx2 = (net.history_index + i) % GRAPH_POINTS;
      
      int8_t rssi1 = net.rssi_history[idx1];
      int8_t rssi2 = net.rssi_history[idx2];
      
      if (rssi1 == 0 || rssi2 == 0) continue;  // Skip uninitialized data
      
      // Map RSSI to Y coordinate (-90 to -30 dBm range)
      rssi1 = constrain(rssi1, -90, -30);
      rssi2 = constrain(rssi2, -90, -30);
      
      uint8_t y1 = graph_y + graph_h - 1 - ((rssi1 + 90) * (graph_h - 1) / 60);
      uint8_t y2 = graph_y + graph_h - 1 - ((rssi2 + 90) * (graph_h - 1) / 60);
      
      uint8_t x1 = graph_x + (i - 1) * graph_w / GRAPH_POINTS;
      uint8_t x2 = graph_x + i * graph_w / GRAPH_POINTS;
      
      // Draw line between points
      drawLine(x1, y1, x2, y2, lineColor);
      
      // Draw larger point at last position
      if (i == GRAPH_POINTS - 1) {
        fillRect(x2 - 1, y2 - 1, 3, 3, lineColor);
      }
    }
    
    networks_drawn++;
  }
}

// Simple line drawing function
void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }
  
  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }
  
  int16_t dx = x1 - x0;
  int16_t dy = abs(y1 - y0);
  int16_t err = dx / 2;
  int16_t ystep = (y0 < y1) ? 1 : -1;
  
  for (; x0 <= x1; x0++) {
    if (steep) {
      fillRect(y0, x0, 1, 1, color);
    } else {
      fillRect(x0, y0, 1, 1, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

void swap(int16_t &a, int16_t &b) {
  int16_t t = a;
  a = b;
  b = t;
}

// Scan WiFi networks
void scanWiFi() {
  Serial.println("Scanning WiFi...");
  int n = WiFi.scanNetworks();
  
  // Mark all as inactive first
  for (uint8_t i = 0; i < MAX_NETWORKS; i++) {
    networks[i].active = false;
  }
  
  network_count = min(n, MAX_NETWORKS);
  
  for (uint8_t i = 0; i < network_count; i++) {
    networks[i].ssid = WiFi.SSID(i);
    networks[i].rssi = WiFi.RSSI(i);
    networks[i].active = true;
    
    // Add to history
    networks[i].rssi_history[networks[i].history_index] = networks[i].rssi;
    networks[i].history_index = (networks[i].history_index + 1) % GRAPH_POINTS;
    
    Serial.printf("%d: %s (%d dBm)\n", i, networks[i].ssid.c_str(), networks[i].rssi);
  }
}

void setup() {
  Serial.begin(115200);
  
  // Initialize display pins
  pinMode(TFT_CS, OUTPUT);
  pinMode(TFT_RST, OUTPUT);
  pinMode(TFT_DC, OUTPUT);
  
  // Initialize SPI
  SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);
  SPI.beginTransaction(SPISettings(40000000, MSBFIRST, SPI_MODE0));
  
  // Reset display
  digitalWrite(TFT_RST, HIGH);
  delay(10);
  digitalWrite(TFT_RST, LOW);
  delay(10);
  digitalWrite(TFT_RST, HIGH);
  delay(150);
  
  // Initialize ST7735
  writeCmd(ST7735_SWRESET);
  delay(150);
  writeCmd(ST7735_SLPOUT);
  delay(200);
  writeCmd(ST7735_INVON);
  writeCmd(ST7735_MADCTL);
  writeData(0x68);
  writeCmd(0x3A);
  writeData(0x05);
  writeCmd(ST7735_DISPON);
  
  // Clear screen
  fillRect(0, 0, W, H, BLACK);
  
  // Initialize WiFi in station mode
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
  // Initialize network structures
  for (uint8_t i = 0; i < MAX_NETWORKS; i++) {
    networks[i].history_index = 0;
    networks[i].active = false;
    for (uint8_t j = 0; j < GRAPH_POINTS; j++) {
      networks[i].rssi_history[j] = 0;
    }
  }
  
  // Initial scan
  scanWiFi();
  drawWiFiList();
  drawGraph();
  
  Serial.println("WiFi Dashboard Ready!");
}

void loop() {
  unsigned long current_time = millis();
  
  // Scan every 1 second
  if (current_time - last_scan >= SCAN_INTERVAL) {
    last_scan = current_time;
    scanWiFi();
    drawWiFiList();
    drawGraph();
  }
  
  delay(50);  // Reduced delay for faster response
}
