# WiFi Dashboard for ST7735 LCD

A real-time WiFi network scanner and signal strength monitor for ESP32 with ST7735 LCD display.

## Features

- Scans and displays up to 8 WiFi networks
- Real-time signal strength monitoring
- Multi-network graph tracking (up to 4 networks simultaneously)
- Color-coded network identification
- Signal history visualization over 26-second period
- Fast refresh rate (1 second scan interval)

## Hardware Requirements

- ESP32 development board
- ST7735 LCD display (160x80 pixels)
- SPI connection

## Pin Configuration

```
TFT_CS    = GPIO 0
TFT_RST   = GPIO 2
TFT_DC    = GPIO 1
TFT_MOSI  = GPIO 21
TFT_SCLK  = GPIO 20
```

## Display Layout

### Left Panel (80x80 pixels)
- WiFi network list with color-coded names
- Signal strength indicators (3-bar icon)
- RSSI values in dBm
- Network names truncated to 6 characters

### Right Panel (80x80 pixels)
- Real-time signal strength graph
- X-axis: Time progression (26 data points)
- Y-axis: Signal strength (-90 dBm to -30 dBm)
- Grid lines every 15 dBm
- Color-matched graph lines for easy identification

## Color Coding

Each network is assigned a unique color that appears in both the network list and graph:

1. Red
2. Orange
3. Yellow
4. Green
5. Cyan
6. Blue
7. Magenta
8. Pink

Signal strength icon colors indicate quality:
- Green: Excellent (>= -50 dBm)
- Light Green: Good (>= -60 dBm)
- Yellow: Fair (>= -70 dBm)
- Orange: Weak (>= -80 dBm)
- Red: Poor (< -80 dBm)

## Technical Specifications

- Scan interval: 1 second
- Display refresh: 50ms loop delay
- Maximum networks displayed: 8
- Networks tracked in graph: 4
- History buffer: 26 points (approximately 26 seconds)
- SSID character limit: 6 characters
- RSSI range: -90 dBm to -30 dBm

## Installation

1. Install Arduino IDE with ESP32 board support
2. Install required library: SPI (included with Arduino)
3. Connect ST7735 LCD to ESP32 according to pin configuration
4. Upload the sketch to ESP32

## Usage

1. Power on the ESP32
2. The display will automatically start scanning for WiFi networks
3. Networks are sorted by signal strength
4. The top 4 networks are tracked in the graph
5. Colors on the left panel match the graph lines on the right

## Code Structure

### Main Components

- **SPI Communication**: Hardware SPI for fast display updates
- **WiFi Scanning**: ESP32 WiFi library for network detection
- **Display Functions**: ST7735 command implementation
- **Graphics Rendering**: Custom text and graphics drawing routines
- **Data Structures**: Network information storage and history buffers

### Key Functions

- `scanWiFi()`: Scans available networks and updates data
- `drawWiFiList()`: Renders network list on left panel
- `drawGraph()`: Renders signal strength graph on right panel
- `drawLine()`: Bresenham line algorithm implementation
- `fillRect()`: Hardware-accelerated rectangle drawing

## Performance Optimizations

- Hardware SPI at 40 MHz for fast display updates
- Optimized drawing routines with minimal data transfers
- Efficient buffer management for history tracking
- Reduced loop delays for responsive updates

## Customization

### Adjustable Parameters

```cpp
#define MAX_NETWORKS 8          // Maximum networks to display
#define GRAPH_POINTS 26         // Number of history points
#define SCAN_INTERVAL 1000      // Scan interval in milliseconds
#define MAX_SSID_CHARS 6        // SSID character limit
#define NETWORKS_TO_GRAPH 4     // Networks to track in graph
```

### Color Palette

Modify the `GRAPH_COLORS` array to change network colors:

```cpp
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
```

## Troubleshooting

### Display Issues
- Verify SPI pin connections
- Check power supply (3.3V or 5V depending on display)
- Adjust X_OFFSET and Y_OFFSET if display alignment is incorrect

### WiFi Scanning Issues
- Ensure ESP32 WiFi antenna is not obstructed
- Check that WiFi networks are using 2.4 GHz band (ESP32 does not support 5 GHz)
- Verify WiFi mode is set to WIFI_STA

### Performance Issues
- Reduce SCAN_INTERVAL if updates are too frequent
- Increase loop delay if CPU usage is too high
- Reduce GRAPH_POINTS for less memory usage

## License

This project is open source and available for modification and distribution.

## Credits

Based on ST7735 display driver and ESP32 WiFi capabilities.
