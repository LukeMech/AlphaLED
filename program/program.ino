// ------------------------
// - Important assigments -
// ------------------------

#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebSrv.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <ESP8266WiFi.h>
#include "FS.h"
#include <NTPClient.h>
#include <WiFiClientSecure.h>
#include <WiFiUdp.h>

const bool WiFi_UpdateCredentialsFile = false;  // Update network_config.txt in filesystem?
const char* ssid = "";                          // Network name
const char* password = "";                      // Network password

const char* host = "github.com";  // Host to check connection, leave as is if using github
const int httpsPort = 443;        // Host port

const char* ntpServerName = "pl.pool.ntp.org";
const int timeZone = 1;

const char* updaterVersionCtrlUrl = "https://raw.githubusercontent.com/LukeMech/AlphaLED/main/updater/build-version.txt";  // Link to version.txt
const char* updaterFirmwareUrl = "https://raw.githubusercontent.com/LukeMech/AlphaLED/main/updater/firmware.bin";          // File to firmware.bin
const char* updaterFSUrl = "https://raw.githubusercontent.com/LukeMech/AlphaLED/main/updater/filesystem.bin";              // File to filesystem.bin

const int LED_PIN = D1;  // LED Pin

Adafruit_NeoPixel strip(64, LED_PIN, NEO_GRB + NEO_KHZ800);  // Init LEDs

uint8_t led_map[8][8] = {  // Table corresponding to the physical position/number of the LEDs
  { 56, 55, 40, 39, 24, 23, 8, 7 },
  { 57, 54, 41, 38, 25, 22, 9, 6 },
  { 58, 53, 42, 37, 26, 21, 10, 5 },
  { 59, 52, 43, 36, 27, 20, 11, 4 },
  { 60, 51, 44, 35, 28, 19, 12, 3 },
  { 61, 50, 45, 34, 29, 18, 13, 2 },
  { 62, 49, 46, 33, 30, 17, 14, 1 },
  { 63, 48, 47, 32, 31, 16, 15, 0 }
};

uint32_t LED_COLOR_0 = strip.Color(0, 0, 0);   // diode color for 0 (background) (R G B)
uint32_t LED_COLOR_1 = strip.Color(25, 0, 0);  // diode color for 1 (text) (R G B)

uint32_t LED_COLOR_CONN = strip.Color(0, 50, 0);  // diode color for update
uint32_t LED_COLOR_UPD = strip.Color(0, 0, 10);   // diode color for update
uint32_t LED_COLOR_ERR = strip.Color(100, 0, 0);  // diode color for update

// Alphabet maps
struct {
  uint8_t

    A[8][8] = { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 1, 1, 1, 1, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },

    B[8][8] = { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 1, 1, 1, 0, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 1, 1, 1, 0, 0, 0 }, { 0, 0, 1, 1, 1, 0, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 1, 1, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    C[8][8] = { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    D[8][8] = { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 1, 1, 1, 0, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 1, 1, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    E[8][8] = { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 1, 1, 1, 1, 0, 0 }, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 0, 0, 1, 1, 1, 0, 0, 0 }, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 0, 0, 1, 1, 1, 1, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },

    F[8][8] = { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 1, 1, 1, 1, 0, 0 }, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 0, 0, 1, 1, 1, 0, 0, 0 }, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    G[8][8] = { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 0, 0, 1, 0, 1, 1, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    H[8][8] = { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 1, 1, 1, 1, 0, 0 }, { 0, 0, 1, 1, 1, 1, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    I[8][8] = { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    J[8][8] = { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 1, 1, 1, 1, 0, 0 }, { 0, 0, 0, 0, 0, 1, 0, 0 }, { 0, 0, 0, 0, 0, 1, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    K[8][8] = { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 1, 0, 1, 0, 0, 0 }, { 0, 0, 1, 1, 0, 0, 0, 0 }, { 0, 0, 1, 1, 0, 0, 0, 0 }, { 0, 0, 1, 0, 1, 0, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    L[8][8] = { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 0, 0, 1, 1, 1, 1, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    Ł[8][8] = { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 0, 0, 1, 0, 1, 0, 0, 0 }, { 0, 0, 1, 1, 0, 0, 0, 0 }, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 0, 0, 1, 1, 1, 1, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    M[8][8] = { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 1, 0, 0, 0, 0, 1, 0 }, { 0, 1, 1, 0, 0, 1, 1, 0 }, { 0, 1, 0, 1, 1, 0, 1, 0 }, { 0, 1, 0, 0, 0, 0, 1, 0 }, { 0, 1, 0, 0, 0, 0, 1, 0 }, { 0, 1, 0, 0, 0, 0, 1, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    N[8][8] = { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 1, 0, 0, 0, 0, 1, 0 }, { 0, 1, 1, 0, 0, 0, 1, 0 }, { 0, 1, 0, 1, 0, 0, 1, 0 }, { 0, 1, 0, 0, 1, 0, 1, 0 }, { 0, 1, 0, 0, 0, 1, 1, 0 }, { 0, 1, 0, 0, 0, 0, 1, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },

    O[8][8] = { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },

    P[8][8] = { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 1, 1, 1, 0, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 1, 1, 1, 0, 0, 0 }, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    R[8][8] = { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 1, 1, 1, 0, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 1, 1, 1, 0, 0, 0 }, { 0, 0, 1, 1, 0, 0, 0, 0 }, { 0, 0, 1, 0, 1, 0, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    S[8][8] = { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 1, 1, 1, 0, 0 }, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 0, 0, 0, 1, 0, 0, 0, 0 }, { 0, 0, 0, 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0, 1, 0, 0 }, { 0, 0, 1, 1, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    T[8][8] = { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 1, 1, 1, 1, 1, 1, 0 }, { 0, 1, 1, 1, 1, 1, 1, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    U[8][8] = { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    W[8][8] = { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 1, 0, 0, 0, 0, 1, 0 }, { 0, 1, 0, 0, 0, 0, 1, 0 }, { 0, 1, 0, 0, 0, 0, 1, 0 }, { 0, 1, 0, 1, 1, 0, 1, 0 }, { 0, 1, 1, 0, 0, 1, 1, 0 }, { 0, 1, 0, 0, 0, 0, 1, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    X[8][8] = { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    Y[8][8] = { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 1, 0, 0, 0, 0, 1, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    Z[8][8] = { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 1, 1, 1, 1, 0, 0 }, { 0, 0, 0, 0, 0, 1, 0, 0 }, { 0, 0, 0, 0, 1, 0, 0, 0 }, { 0, 0, 0, 1, 0, 0, 0, 0 }, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 0, 0, 1, 1, 1, 1, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    Ź[8][8] = { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 1, 0, 0, 0 }, { 0, 0, 0, 1, 0, 0, 0, 0 }, { 0, 0, 1, 1, 1, 1, 0, 0 }, { 0, 0, 0, 0, 1, 0, 0, 0 }, { 0, 0, 0, 1, 0, 0, 0, 0 }, { 0, 0, 1, 1, 1, 1, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    Ż[8][8] = { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 1, 1, 1, 1, 0, 0 }, { 0, 0, 0, 0, 1, 0, 0, 0 }, { 0, 0, 0, 1, 0, 0, 0, 0 }, { 0, 0, 1, 1, 1, 1, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } };

} alphabet;

// Characters maps
struct {
  uint8_t

    space[8][8] = { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    exclam_mark[8][8] = { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    updater[8][8] = { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 1, 1, 1, 1, 0, 0 }, { 0, 1, 1, 1, 1, 1, 1, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } };

} characters;

// Numbers maps
struct {
  uint8_t
    one[8][8] = { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } };

} numbers;

AsyncWebServer server(80);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServerName, timeZone);

int8_t patternNum = 0;
bool serverOn = false, updateFirmware = false;



// ------------------------
// --------- LEDs ---------
// ------------------------

// Display static map
void display(uint8_t map[][8]) {
  for (uint8_t i = 0; i < 8; i++) {
    for (uint8_t j = 0; j < 8; j++) {
      uint8_t value = map[i][j];
      uint8_t index = led_map[i][j];
      if (value == 0) strip.setPixelColor(index, LED_COLOR_0);
      else if (value == 1) strip.setPixelColor(index, LED_COLOR_1);
    }
  }
  strip.show();
}

// Animate between maps
void animate(uint8_t startMap[][8], uint8_t endMap[][8], uint8_t direction = 0, int gap = 50, uint32_t newColor1 = 0, uint32_t newColor0 = 0) {

  uint32_t oldColor0 = LED_COLOR_0, oldColor1 = LED_COLOR_1, color0, color1;
  // New background color
  if (newColor0) LED_COLOR_0 = newColor0;
  // New text color
  if (newColor1) LED_COLOR_1 = newColor1;

  // Direction 0: <<<---- Text
  if (!direction) {
    for (uint8_t shift = 1; shift < 8; shift++) {
      for (uint8_t y = 0; y < 8; y++) {
        for (uint8_t x = 0; x < 8; x++) {
          uint8_t value = 0;
          if (x < (7 - shift)) {
            value = startMap[y][x + shift];
            color0 = oldColor0;
            color1 = oldColor1;
          } else {
            value = endMap[y][x - (7 - shift)];
            color0 = LED_COLOR_0;
            color1 = LED_COLOR_1;
          }
          uint8_t index = led_map[y][x];
          if (value == 0) strip.setPixelColor(index, color0);
          else if (value == 1) strip.setPixelColor(index, color1);
        }
      }
      strip.show();
      delay(gap);
    }
  }
  // Direction 1: Text ---->>>
  else if (direction == 1) {
    for (int8_t shift = 6; shift >= 0; shift--) {
      for (uint8_t y = 0; y < 8; y++) {
        for (uint8_t x = 0; x < 8; x++) {
          uint8_t value = 0;
          if (x >= (7 - shift)) {
            value = startMap[y][x - (7 - shift)];
            color0 = oldColor0;
            color1 = oldColor1;
          } else {
            value = endMap[y][x + shift];
            color0 = LED_COLOR_0;
            color1 = LED_COLOR_1;
          }
          uint8_t index = led_map[y][x];
          if (value == 0) strip.setPixelColor(index, color0);
          else if (value == 1) strip.setPixelColor(index, color1);
        }
      }
      strip.show();
      delay(gap);
    }
  }
  // Direction 2:  ^ Text ^
  else if (direction == 2) {
    for (uint8_t shift = 1; shift < 8; shift++) {
      for (uint8_t x = 0; x < 8; x++) {
        for (uint8_t y = 0; y < 8; y++) {
          uint8_t value = 0;
          if (y < (7 - shift)) {
            value = startMap[y + shift][x];
            color0 = oldColor0;
            color1 = oldColor1;
          } else {
            value = endMap[y - (7 - shift)][x];
            color0 = LED_COLOR_0;
            color1 = LED_COLOR_1;
          }
          uint8_t index = led_map[y][x];
          if (value == 0) strip.setPixelColor(index, color0);
          else if (value == 1) strip.setPixelColor(index, color1);
        }
      }
      strip.show();
      delay(gap);
    }
  }
  // Direction 3:  \/ Text \/
  else if (direction == 3) {
    for (int8_t shift = 6; shift >= 0; shift--) {
      for (uint8_t x = 0; x < 8; x++) {
        for (uint8_t y = 0; y < 8; y++) {
          uint8_t value = 0;
          if (y >= (7 - shift)) {
            value = startMap[y - (7 - shift)][x];
            color0 = oldColor0;
            color1 = oldColor1;
          } else {
            value = endMap[y + shift][x];
            color0 = LED_COLOR_0;
            color1 = LED_COLOR_1;
          }
          uint8_t index = led_map[y][x];
          if (value == 0) strip.setPixelColor(index, color0);
          else if (value == 1) strip.setPixelColor(index, color1);
        }
      }
      strip.show();
      delay(gap);
    }
  }
}

void alphabetAnim(int gaps = 0, int animLength = 90) {

  if (!updateFirmware) animate(alphabet.A, alphabet.B, 0, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.B, alphabet.C, 0, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.C, alphabet.D, 0, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.D, alphabet.E, 0, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.E, alphabet.F, 0, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.F, alphabet.G, 0, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.G, alphabet.H, 0, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.H, alphabet.I, 0, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.I, alphabet.J, 0, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.J, alphabet.K, 0, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.K, alphabet.L, 0, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.L, alphabet.M, 0, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.M, alphabet.N, 0, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.N, alphabet.O, 0, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.O, alphabet.P, 0, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.P, alphabet.R, 0, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.R, alphabet.S, 0, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.S, alphabet.T, 0, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.T, alphabet.U, 0, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.U, alphabet.W, 0, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.W, alphabet.X, 0, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.X, alphabet.Y, 0, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.Y, alphabet.Z, 0, animLength);
  if (!updateFirmware) delay(gaps ? gaps * 2 : 200);

  if (!updateFirmware) animate(alphabet.Z, alphabet.Y, 1, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.Y, alphabet.X, 1, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.X, alphabet.W, 1, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.W, alphabet.U, 1, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.U, alphabet.T, 1, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.T, alphabet.S, 1, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.S, alphabet.R, 1, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.R, alphabet.P, 1, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.P, alphabet.O, 1, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.O, alphabet.N, 1, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.N, alphabet.M, 1, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.M, alphabet.L, 1, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.L, alphabet.K, 1, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.K, alphabet.J, 1, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.J, alphabet.I, 1, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.I, alphabet.H, 1, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.H, alphabet.G, 1, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.G, alphabet.F, 1, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.F, alphabet.E, 1, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.E, alphabet.D, 1, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.D, alphabet.C, 1, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.C, alphabet.B, 1, animLength);
  if (!updateFirmware) delay(gaps);
  if (!updateFirmware) animate(alphabet.B, alphabet.A, 1, animLength);
  if (!updateFirmware) delay(gaps ? gaps * 2 : 200);
}

void cba() {
  if(!updateFirmware) animate(alphabet.C, alphabet.B, 2, 120, strip.Color(0, 0, 25));
  if(!updateFirmware) animate(alphabet.B, alphabet.A, 2, 120, strip.Color(0, 25, 0));
  if(!updateFirmware) animate(alphabet.A, alphabet.B, 3, 120, strip.Color(0, 0, 25));
  if(!updateFirmware) animate(alphabet.B, alphabet.C, 3, 120, strip.Color(25, 0, 0));
}

void flashlight(float brightness) {
  strip.fill(strip.Color(brightness*0.25*255, brightness*0.25*255, brightness*0.25*255));
  for(int s=0; s<8; s++) strip.setPixelColor(led_map[0][s], strip.Color(brightness*0.6*255, brightness*0.6*255, brightness*0.6*255));
  for(int e=0; e<8; e++) strip.setPixelColor(led_map[7][e], strip.Color(brightness*0.6*255, brightness*0.6*255, brightness*0.6*255));
  for(int x=0; x<8; x++) strip.setPixelColor(led_map[x][0], strip.Color(brightness*0.6*255, brightness*0.6*255, brightness*0.6*255));
  for(int y=0; y<8; y++) strip.setPixelColor(led_map[y][7], strip.Color(brightness*0.6*255, brightness*0.6*255, brightness*0.6*255));
  strip.show();
}



// ------------------------
// ------- Updater --------
// ------------------------

#include <CertStoreBearSSL.h>
BearSSL::CertStore certStore;

// DigiCert High Assurance EV Root CA
const char trustRoot[] PROGMEM = R"EOF( 
-----BEGIN CERTIFICATE-----
MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD
QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j
b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG
9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB
CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97
nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt
43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P
T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4
gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO
BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR
TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw
DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr
hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg
06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF
PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls
YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk
CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=
-----END CERTIFICATE-----
)EOF";
X509List cert(trustRoot);


//                      <--- WiFi connector --->

struct WiFiCredentials {
  char ssid[32];
  char password[32];
};

void saveWiFiCredentials(const char* ssid, const char* password) {
  WiFiCredentials credentials;

  strncpy(credentials.ssid, ssid, 32);
  strncpy(credentials.password, password, 32);

  EEPROM.begin(sizeof(credentials));
  EEPROM.put(0, credentials);
  EEPROM.commit();
  EEPROM.end();
}

bool loadWiFiCredentials(char* ssid, char* password) {
  WiFiCredentials credentials;

  EEPROM.begin(sizeof(credentials));
  EEPROM.get(0, credentials);
  EEPROM.end();

  if (strlen(credentials.ssid) == 0 || strlen(credentials.password) == 0) {
    return false;
  }

  strncpy(ssid, credentials.ssid, 32);
  strncpy(password, credentials.password, 32);

  return true;
}

void wiFiInit() {
  char savedSsid[32];
  char savedPass[32];
  if (loadWiFiCredentials(savedSsid, savedPass)) {
    WiFi.begin(savedSsid, savedPass);  // If yes, try to connect
    Serial.print("[INFO] Connecting to: ");
    Serial.println(savedSsid);

  } else Serial.print("[ERROR] No wifi info saved.");
}


//                      <--- Firmware updater --->

void firmwareUpdate() {  // Updater

  display(characters.space);
  strip.setPixelColor(led_map[0][0], LED_COLOR_UPD);
  strip.show();

  WiFiClientSecure client;  // Create secure wifi client
  client.setTrustAnchors(&cert);

  timeClient.update();

  if (!client.connect(host, httpsPort)) { // Connect to github
    Serial.println("[ERROR] Connection to github unavailable");
    return;  
  }

  HTTPClient http;  // Connect to release API
  http.begin(client, updaterVersionCtrlUrl);
  int httpCode = http.GET();
  if (httpCode != HTTP_CODE_OK) {
    Serial.println("[ERROR] Cannot check server versionfile");
    return;
  }

  String new_version = http.getString();  //Download version tag
  int firmware_pos = new_version.indexOf("Firmware: ") + 10;
  String newFirmwareVer = new_version.substring(firmware_pos);
  int fs_pos = new_version.indexOf("Filesystem: ") + 12;
  int fs_end_pos = new_version.indexOf("\nFirmware:");
  String newFsVer = new_version.substring(fs_pos, fs_end_pos);
  newFsVer.trim();
  newFirmwareVer.trim();
  http.end();

  String firmwareVer, fsVer;
  File file = SPIFFS.open("/version.txt", "r");  // Read versions
  while (file.available()) {
    String line = file.readStringUntil('\n');
    if (line.startsWith("Filesystem:")) fsVer = line.substring(line.indexOf(":") + 2);
    else if (line.startsWith("Firmware:")) firmwareVer = line.substring(line.indexOf(":") + 2);
  }
  fsVer.trim();
  firmwareVer.trim();
  file.close();

  Serial.println("[INFO] Firmware version: " + firmwareVer);
  Serial.println("[INFO] Firmware latest version: " + newFirmwareVer);
  Serial.println("[INFO] Local filesystem version: " + fsVer);
  Serial.println("[INFO] Filesystem latest version: " + newFsVer);

  // Check if version is the same
  bool updateFS = true, updateFirmware = true;
  if (!strcmp(firmwareVer.c_str(), newFirmwareVer.c_str()) || (!newFirmwareVer.c_str() || newFirmwareVer.c_str() == "")) updateFirmware = false;
  if (!strcmp(fsVer.c_str(), newFsVer.c_str()) || (!newFsVer.c_str() || newFsVer.c_str() == "")) updateFS = false;

  // If up-to-date
  if (!updateFirmware && !updateFS) return;

  // Set LEDs
  strip.setPixelColor(led_map[0][0], LED_COLOR_0);
  if (updateFirmware && updateFS) {
    for (uint8_t i; i < 8; i++) strip.setPixelColor(led_map[0][i], LED_COLOR_UPD);
    for (uint8_t i; i < 8; i++) strip.setPixelColor(led_map[3][i], LED_COLOR_UPD);
    for (uint8_t i; i < 8; i++) strip.setPixelColor(led_map[4][i], LED_COLOR_UPD);
    for (uint8_t i; i < 8; i++) strip.setPixelColor(led_map[7][i], LED_COLOR_UPD);
    for (uint8_t i; i < 8; i++) strip.setPixelColor(led_map[i][0], LED_COLOR_UPD);
    for (uint8_t i; i < 8; i++) strip.setPixelColor(led_map[i][7], LED_COLOR_UPD);
  } else {
    for (uint8_t i; i < 8; i++) strip.setPixelColor(led_map[2][i], LED_COLOR_UPD);
    for (uint8_t i; i < 8; i++) strip.setPixelColor(led_map[5][i], LED_COLOR_UPD);
    strip.setPixelColor(led_map[3][0], LED_COLOR_UPD);
    strip.setPixelColor(led_map[4][0], LED_COLOR_UPD);
    strip.setPixelColor(led_map[3][7], LED_COLOR_UPD);
    strip.setPixelColor(led_map[4][7], LED_COLOR_UPD);
  }
  strip.show();

  // Update progress - change leds
  bool secStage = false, dualUpdate = false;
  if (updateFirmware && updateFS) dualUpdate = true;
  ESPhttpUpdate.rebootOnUpdate(false);
  ESPhttpUpdate.onStart([]() {
    Serial.println("Starting update...");
  });

  ESPhttpUpdate.onProgress([&](int current, int total) {
    float progress = (float)current / (float)total;
    int value = round(progress * 4) + 1;
    Serial.print("[PROGRESS] Updater: ");
    Serial.println(progress * 100);

    strip.setPixelColor(led_map[dualUpdate ? (secStage ? 5 : 1) : 3][value], LED_COLOR_CONN);
    strip.setPixelColor(led_map[dualUpdate ? (secStage ? 6 : 2) : 4][value], LED_COLOR_CONN);
    strip.show();
  });

  ESPhttpUpdate.onEnd([&]() {
    strip.setPixelColor(led_map[dualUpdate ? (secStage ? 5 : 1) : 3][6], LED_COLOR_CONN);
    strip.setPixelColor(led_map[dualUpdate ? (secStage ? 6 : 2) : 4][6], LED_COLOR_CONN);
    if (!updateFS) {
      File versionFile = SPIFFS.open("/version.txt", "w");
      versionFile.seek(0);
      versionFile.write(new_version.c_str());
      Serial.println("[INFO] Updating version.txt based on http string:\n-----");
      Serial.println(new_version.c_str());
      Serial.println("-----");
      versionFile.close();
    }
    strip.show();
  });

  t_httpUpdate_return ret;
  if (updateFirmware) ret = ESPhttpUpdate.update(client, updaterFirmwareUrl);  // Update firmware
  secStage = true;
  SPIFFS.end();

  if (updateFS && (!updateFirmware || ret == HTTP_UPDATE_OK || ret == 0)) ret = ESPhttpUpdate.updateFS(client, updaterFSUrl);  // Update filesystem

  if (ret != HTTP_UPDATE_OK && ret != 0) {  // Error
    Serial.print("[ERROR] ");
    Serial.println(ESPhttpUpdate.getLastErrorString());
    Serial.println(ret);
    strip.setPixelColor(led_map[dualUpdate ? (secStage ? 5 : 1) : 3][0], LED_COLOR_ERR);
    strip.setPixelColor(led_map[dualUpdate ? (secStage ? 6 : 2) : 4][0], LED_COLOR_ERR);

    strip.setPixelColor(led_map[dualUpdate ? (secStage ? 5 : 1) : 3][7], LED_COLOR_ERR);
    strip.setPixelColor(led_map[dualUpdate ? (secStage ? 6 : 2) : 4][7], LED_COLOR_ERR);
    strip.show();
    delay(2000);

  } else {
    delay(500);
    animate(characters.space, characters.updater, 2, 100, LED_COLOR_CONN);
    delay(500);
  }

  ESP.restart();  // End update
}



// ------------------------
// ------- Server ---------
// ------------------------

void initServer() {

  // Home site and reuired additional htmls, csss and jss
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/html/index.html", "text/html");
  });
  server.on("/version.txt", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/version.txt", "text/plain");
  });
  server.on("/html/footer.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/html/footer.html", "text/html");
  });
  server.on("/style/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/style/style.css", "text/css");
  });
  server.on("/style/footer.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/style/footer.css", "text/css");
  });
  server.on("/scripts/main.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/scripts/main.js", "text/js");
  });
  server.on("/scripts/script.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/scripts/script.js", "text/js");
  });
  server.on("/images/logo.png", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/images/logo.png", String(), true);
  });
  server.on("/images/blackhole.jpg", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/images/blackhole.jpg", String(), true);
  });

  // Info site
  server.on("/info", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/html/info.html", "text/html");
  });
  server.on("/scripts/info.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/scripts/info.js", "text/js");
  });
  server.on("/images/cosmos.jpg", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/images/cosmos.jpg", String(), true);
  });
  server.on("/functions/getOSinfo", HTTP_GET, [](AsyncWebServerRequest *request) {
    File file = SPIFFS.open("/version.txt", "r");  // Read versions
    String version = file.readString();
    file.close();
    timeClient.update();
    String textToReturn = version + "\n" + "Chip ID: " + String(ESP.getChipId()) + "\n" + String(timeClient.getFormattedTime());
    request->send(200, "text/plain", textToReturn);
  });

  // Auto-refresh animation pattern
  server.on("/functions/getLedsPattern", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(patternNum + 1));
  });

  // Change pattern
  server.on("/functions/change", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("Received change command");
    patternNum++;
    if (patternNum<0 || patternNum > 1) patternNum = 0;
    request->redirect("/");
  });

  server.on("/functions/flashlight", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("Received flashlight command");
    if(patternNum<0 && patternNum>-4) patternNum--;    
    else patternNum = -1;
    request->redirect("/");
  });

  // Updater
  server.on("/functions/update", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("Received update command");
    request->redirect("/");
    updateFirmware = true;
  });

  server.begin();

  Serial.print("[INFO] Server IP: ");
  Serial.println(WiFi.localIP());

  serverOn = true;
}



// ------------------------
// ----- Setup & loop -----
// ------------------------

void setup() {
  Serial.begin(9600);  // Begin serial
  Serial.println("[STATUS] Start!");

  strip.begin();  // Init strips
  display(characters.space);

  if (!SPIFFS.begin()) ESP.restart();  // Begin filesystem

  pinMode(LED_BUILTIN, OUTPUT);  // Set pin modes
  digitalWrite(LED_BUILTIN, HIGH);

  if (WiFi_UpdateCredentialsFile) saveWiFiCredentials(ssid, password);  // Save network config
  wiFiInit();                                                           // Connect to wifi
}

void loop() {

  if (patternNum == 0) cba();
  else if (patternNum == 1) alphabetAnim();

  else if (patternNum == -1) flashlight(0.2);
  else if (patternNum == -2) flashlight(0.35);
  else if (patternNum == -3) flashlight(0.5);
  else if (patternNum == -4) flashlight(0.8);

  if (updateFirmware) {
    server.end();
    firmwareUpdate();  // Update firmware if server requested
    updateFirmware = false;
    delay(2000);
    server.begin();
  }

  if (WiFi.status() == WL_CONNECTED && !serverOn) {
    timeClient.begin();
    while(!timeClient.update()) {
      timeClient.forceUpdate();
    }
    initServer();  // Start server if wifi initialized
  }
}