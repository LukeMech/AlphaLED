// ------------------------
// - Important assigments -
// ------------------------

#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebSrv.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <ESP8266WiFi.h>
#include "FS.h"
#include <time.h>
#include <WiFiClientSecure.h>

const bool WiFi_UpdateCredentialsFile = false; // Update network_config.txt in filesystem?
const char *ssid = "";                         // Network name
const char *password = "";                     // Network password

const int LED_PIN = D1; // LED Pin

Adafruit_NeoPixel strip(64, LED_PIN, NEO_GRB + NEO_KHZ800); // Init LEDs

const uint8_t led_map[8][8] = { // Table corresponding to the physical position/number of the LEDs
    {56, 55, 40, 39, 24, 23, 8, 7},
    {57, 54, 41, 38, 25, 22, 9, 6},
    {58, 53, 42, 37, 26, 21, 10, 5},
    {59, 52, 43, 36, 27, 20, 11, 4},
    {60, 51, 44, 35, 28, 19, 12, 3},
    {61, 50, 45, 34, 29, 18, 13, 2},
    {62, 49, 46, 33, 30, 17, 14, 1},
    {63, 48, 47, 32, 31, 16, 15, 0}};

uint32_t LED_COLOR_0 = strip.Color(0, 0, 0);  // diode color for 0 (background) (R G B)
uint32_t LED_COLOR_1 = strip.Color(25, 0, 0); // diode color for 1 (text) (R G B)

const uint32_t LED_COLOR_CONN = strip.Color(0, 50, 0); // diode color for update
const uint32_t LED_COLOR_UPD = strip.Color(0, 0, 10);  // diode color for update
const uint32_t LED_COLOR_ERR = strip.Color(100, 0, 0); // diode color for update

int8_t patternNum = 0;
byte flashlightColorR = 255, flashlightColorG = 255, flashlightColorB = 255;
float flashlightBrightness = 0;
bool serverOn = false;
char updateFS[150];
char updateFv[150];
char versionString[60];

// Alphabet maps
const struct
{
  const uint8_t

      A[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 1, 0, 0, 1, 0, 0}, {0, 0, 1, 0, 0, 1, 0, 0}, {0, 0, 1, 1, 1, 1, 0, 0}, {0, 0, 1, 0, 0, 1, 0, 0}, {0, 0, 1, 0, 0, 1, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},

      B[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 1, 1, 1, 0, 0, 0}, {0, 0, 1, 0, 0, 1, 0, 0}, {0, 0, 1, 1, 1, 0, 0, 0}, {0, 0, 1, 1, 1, 0, 0, 0}, {0, 0, 1, 0, 0, 1, 0, 0}, {0, 0, 1, 1, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
      C[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 1, 0, 0, 1, 0, 0}, {0, 0, 1, 0, 0, 0, 0, 0}, {0, 0, 1, 0, 0, 0, 0, 0}, {0, 0, 1, 0, 0, 1, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
      D[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 1, 1, 1, 0, 0, 0}, {0, 0, 1, 0, 0, 1, 0, 0}, {0, 0, 1, 0, 0, 1, 0, 0}, {0, 0, 1, 0, 0, 1, 0, 0}, {0, 0, 1, 0, 0, 1, 0, 0}, {0, 0, 1, 1, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
      E[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 1, 1, 1, 1, 0, 0}, {0, 0, 1, 0, 0, 0, 0, 0}, {0, 0, 1, 0, 0, 0, 0, 0}, {0, 0, 1, 1, 1, 0, 0, 0}, {0, 0, 1, 0, 0, 0, 0, 0}, {0, 0, 1, 1, 1, 1, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},

      F[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 1, 1, 1, 1, 0, 0}, {0, 0, 1, 0, 0, 0, 0, 0}, {0, 0, 1, 1, 1, 0, 0, 0}, {0, 0, 1, 0, 0, 0, 0, 0}, {0, 0, 1, 0, 0, 0, 0, 0}, {0, 0, 1, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
      G[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 1, 0, 0, 1, 0, 0}, {0, 0, 1, 0, 0, 0, 0, 0}, {0, 0, 1, 0, 1, 1, 0, 0}, {0, 0, 1, 0, 0, 1, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
      H[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 1, 0, 0, 1, 0, 0}, {0, 0, 1, 0, 0, 1, 0, 0}, {0, 0, 1, 1, 1, 1, 0, 0}, {0, 0, 1, 1, 1, 1, 0, 0}, {0, 0, 1, 0, 0, 1, 0, 0}, {0, 0, 1, 0, 0, 1, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
      I[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
      J[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 1, 1, 1, 1, 0, 0}, {0, 0, 0, 0, 0, 1, 0, 0}, {0, 0, 0, 0, 0, 1, 0, 0}, {0, 0, 1, 0, 0, 1, 0, 0}, {0, 0, 1, 0, 0, 1, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
      K[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 1, 0, 0, 1, 0, 0}, {0, 0, 1, 0, 1, 0, 0, 0}, {0, 0, 1, 1, 0, 0, 0, 0}, {0, 0, 1, 1, 0, 0, 0, 0}, {0, 0, 1, 0, 1, 0, 0, 0}, {0, 0, 1, 0, 0, 1, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
      L[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 1, 0, 0, 0, 0, 0}, {0, 0, 1, 0, 0, 0, 0, 0}, {0, 0, 1, 0, 0, 0, 0, 0}, {0, 0, 1, 0, 0, 0, 0, 0}, {0, 0, 1, 0, 0, 0, 0, 0}, {0, 0, 1, 1, 1, 1, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
      Ł[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 1, 0, 0, 0, 0, 0}, {0, 0, 1, 0, 0, 0, 0, 0}, {0, 0, 1, 0, 1, 0, 0, 0}, {0, 0, 1, 1, 0, 0, 0, 0}, {0, 0, 1, 0, 0, 0, 0, 0}, {0, 0, 1, 1, 1, 1, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
      M[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 1, 0, 0, 0, 0, 1, 0}, {0, 1, 1, 0, 0, 1, 1, 0}, {0, 1, 0, 1, 1, 0, 1, 0}, {0, 1, 0, 0, 0, 0, 1, 0}, {0, 1, 0, 0, 0, 0, 1, 0}, {0, 1, 0, 0, 0, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
      N[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 1, 0, 0, 0, 0, 1, 0}, {0, 1, 1, 0, 0, 0, 1, 0}, {0, 1, 0, 1, 0, 0, 1, 0}, {0, 1, 0, 0, 1, 0, 1, 0}, {0, 1, 0, 0, 0, 1, 1, 0}, {0, 1, 0, 0, 0, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},

      O[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 1, 0, 0, 1, 0, 0}, {0, 0, 1, 0, 0, 1, 0, 0}, {0, 0, 1, 0, 0, 1, 0, 0}, {0, 0, 1, 0, 0, 1, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},

      P[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 1, 1, 1, 0, 0, 0}, {0, 0, 1, 0, 0, 1, 0, 0}, {0, 0, 1, 1, 1, 0, 0, 0}, {0, 0, 1, 0, 0, 0, 0, 0}, {0, 0, 1, 0, 0, 0, 0, 0}, {0, 0, 1, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
      R[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 1, 1, 1, 0, 0, 0}, {0, 0, 1, 0, 0, 1, 0, 0}, {0, 0, 1, 1, 1, 0, 0, 0}, {0, 0, 1, 1, 0, 0, 0, 0}, {0, 0, 1, 0, 1, 0, 0, 0}, {0, 0, 1, 0, 0, 1, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
      S[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 1, 1, 1, 0, 0}, {0, 0, 1, 0, 0, 0, 0, 0}, {0, 0, 0, 1, 0, 0, 0, 0}, {0, 0, 0, 0, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 1, 0, 0}, {0, 0, 1, 1, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
      T[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 1, 1, 1, 1, 1, 1, 0}, {0, 1, 1, 1, 1, 1, 1, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
      U[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 1, 0, 0, 1, 0, 0}, {0, 0, 1, 0, 0, 1, 0, 0}, {0, 0, 1, 0, 0, 1, 0, 0}, {0, 0, 1, 0, 0, 1, 0, 0}, {0, 0, 1, 0, 0, 1, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
      V[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 1, 0, 0, 1, 0, 0}, {0, 0, 1, 0, 0, 1, 0, 0}, {0, 1, 0, 0, 0, 0, 1, 0}, {0, 1, 0, 0, 0, 0, 1, 0}, {0, 1, 0, 0, 0, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
      W[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 1, 0, 0, 0, 0, 1, 0}, {0, 1, 0, 0, 0, 0, 1, 0}, {0, 1, 0, 0, 0, 0, 1, 0}, {0, 1, 0, 1, 1, 0, 1, 0}, {0, 1, 1, 0, 0, 1, 1, 0}, {0, 1, 0, 0, 0, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
      X[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 1, 0, 0, 1, 0, 0}, {0, 0, 1, 0, 0, 1, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 1, 0, 0, 1, 0, 0}, {0, 0, 1, 0, 0, 1, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
      Y[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 1, 0, 0, 0, 0, 1, 0}, {0, 0, 1, 0, 0, 1, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
      Z[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 1, 1, 1, 1, 0, 0}, {0, 0, 0, 0, 0, 1, 0, 0}, {0, 0, 0, 0, 1, 0, 0, 0}, {0, 0, 0, 1, 0, 0, 0, 0}, {0, 0, 1, 0, 0, 0, 0, 0}, {0, 0, 1, 1, 1, 1, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
      Ź[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 1, 0, 0, 0}, {0, 0, 0, 1, 0, 0, 0, 0}, {0, 0, 1, 1, 1, 1, 0, 0}, {0, 0, 0, 0, 1, 0, 0, 0}, {0, 0, 0, 1, 0, 0, 0, 0}, {0, 0, 1, 1, 1, 1, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
      Ż[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 1, 1, 1, 1, 0, 0}, {0, 0, 0, 0, 1, 0, 0, 0}, {0, 0, 0, 1, 0, 0, 0, 0}, {0, 0, 1, 1, 1, 1, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}};

} alphabet;

// Characters maps
const struct
{
  const uint8_t

      space[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
      exclam_mark[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}},
      updater[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 1, 1, 1, 1, 0, 0}, {0, 1, 1, 1, 1, 1, 1, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}};

} characters;

// Numbers maps
const struct
{
  const uint8_t
      one[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}};

} numbers;

// Return maps
typedef const uint8_t (*arrayPtr)[8];
arrayPtr characterToMap(String value)
{
  if (value == "A")
    return alphabet.A;
  if (value == "Ą")
    return alphabet.A;
  else if (value == "B")
    return alphabet.B;
  else if (value == "C")
    return alphabet.C;
  else if (value == "Ć")
    return alphabet.C;
  else if (value == "D")
    return alphabet.D;
  else if (value == "E")
    return alphabet.E;
  else if (value == "Ę")
    return alphabet.E;
  else if (value == "F")
    return alphabet.F;
  else if (value == "G")
    return alphabet.G;
  else if (value == "H")
    return alphabet.H;
  else if (value == "I")
    return alphabet.I;
  else if (value == "J")
    return alphabet.J;
  else if (value == "K")
    return alphabet.K;
  else if (value == "L")
    return alphabet.L;
  else if (value == "Ł")
    return alphabet.Ł;
  else if (value == "M")
    return alphabet.M;
  else if (value == "N")
    return alphabet.N;
  else if (value == "O")
    return alphabet.O;
  else if (value == "Ó")
    return alphabet.O;
  else if (value == "P")
    return alphabet.P;
  else if (value == "R")
    return alphabet.R;
  else if (value == "S")
    return alphabet.S;
  else if (value == "Ś")
    return alphabet.S;
  else if (value == "T")
    return alphabet.T;
  else if (value == "U")
    return alphabet.U;
  else if (value == "W")
    return alphabet.W;
  else if (value == "V")
    return alphabet.V;
  else if (value == "X")
    return alphabet.X;
  else if (value == "Y")
    return alphabet.Y;
  else if (value == "Z")
    return alphabet.Z;
  else if (value == "Ź")
    return alphabet.Ź;
  else if (value == "Ż")
    return alphabet.Ż;

  else if (value == " ")
    return characters.space;
  else if (value == "!")
    return characters.exclam_mark;

  else if (value == "1")
    return numbers.one;

  return characters.space;
}

AsyncWebServer server(80);

// ------------------------
// --------- LEDs ---------
// ------------------------

// Display static map
DynamicJsonDocument displayPatternJson(2048);

void display(const uint8_t map[][8])
{
  for (uint8_t i = 0; i < 8; i++)
  {
    for (uint8_t j = 0; j < 8; j++)
    {
      uint8_t value = map[i][j];
      uint8_t index = led_map[i][j];
      if (value == 0)
        strip.setPixelColor(index, LED_COLOR_0);
      else if (value == 1)
        strip.setPixelColor(index, LED_COLOR_1);
    }
  }
  strip.show();
}

// Animate between maps
void animate(const uint8_t startMap[][8], const uint8_t endMap[][8], uint8_t direction = 0, int gap = 50, uint32_t newColor1 = 0, uint32_t newColor0 = 0)
{

  uint32_t oldColor0 = LED_COLOR_0, oldColor1 = LED_COLOR_1, color0, color1;
  // New background color
  if (newColor0)
    LED_COLOR_0 = newColor0;
  // New text color
  if (newColor1)
    LED_COLOR_1 = newColor1;

  // Direction 0: <<<---- Text
  if (!direction)
  {
    for (uint8_t shift = 1; shift < 8; shift++)
    {
      for (uint8_t y = 0; y < 8; y++)
      {
        for (uint8_t x = 0; x < 8; x++)
        {
          uint8_t value = 0;
          if (x < (7 - shift))
          {
            value = startMap[y][x + shift];
            color0 = oldColor0;
            color1 = oldColor1;
          }
          else
          {
            value = endMap[y][x - (7 - shift)];
            color0 = LED_COLOR_0;
            color1 = LED_COLOR_1;
          }
          uint8_t index = led_map[y][x];
          if (value == 0)
            strip.setPixelColor(index, color0);
          else if (value == 1)
            strip.setPixelColor(index, color1);
        }
      }
      strip.show();
      delay(gap);
    }
  }
  // Direction 1: Text ---->>>
  else if (direction == 1)
  {
    for (int8_t shift = 6; shift >= 0; shift--)
    {
      for (uint8_t y = 0; y < 8; y++)
      {
        for (uint8_t x = 0; x < 8; x++)
        {
          uint8_t value = 0;
          if (x >= (7 - shift))
          {
            value = startMap[y][x - (7 - shift)];
            color0 = oldColor0;
            color1 = oldColor1;
          }
          else
          {
            value = endMap[y][x + shift];
            color0 = LED_COLOR_0;
            color1 = LED_COLOR_1;
          }
          uint8_t index = led_map[y][x];
          if (value == 0)
            strip.setPixelColor(index, color0);
          else if (value == 1)
            strip.setPixelColor(index, color1);
        }
      }
      strip.show();
      delay(gap);
    }
  }
  // Direction 2:  ^ Text ^
  else if (direction == 2)
  {
    for (uint8_t shift = 1; shift < 8; shift++)
    {
      for (uint8_t x = 0; x < 8; x++)
      {
        for (uint8_t y = 0; y < 8; y++)
        {
          uint8_t value = 0;
          if (y < (7 - shift))
          {
            value = startMap[y + shift][x];
            color0 = oldColor0;
            color1 = oldColor1;
          }
          else
          {
            value = endMap[y - (7 - shift)][x];
            color0 = LED_COLOR_0;
            color1 = LED_COLOR_1;
          }
          uint8_t index = led_map[y][x];
          if (value == 0)
            strip.setPixelColor(index, color0);
          else if (value == 1)
            strip.setPixelColor(index, color1);
        }
      }
      strip.show();
      delay(gap);
    }
  }
  // Direction 3:  \/ Text \/
  else if (direction == 3)
  {
    for (int8_t shift = 6; shift >= 0; shift--)
    {
      for (uint8_t x = 0; x < 8; x++)
      {
        for (uint8_t y = 0; y < 8; y++)
        {
          uint8_t value = 0;
          if (y >= (7 - shift))
          {
            value = startMap[y - (7 - shift)][x];
            color0 = oldColor0;
            color1 = oldColor1;
          }
          else
          {
            value = endMap[y + shift][x];
            color0 = LED_COLOR_0;
            color1 = LED_COLOR_1;
          }
          uint8_t index = led_map[y][x];
          if (value == 0)
            strip.setPixelColor(index, color0);
          else if (value == 1)
            strip.setPixelColor(index, color1);
        }
      }
      strip.show();
      delay(gap);
    }
  }
}

void mainAnimation()
{
  animate(characterToMap("C"), characterToMap("B"), 2, 120, strip.Color(0, 0, 25));
  animate(characterToMap("B"), characterToMap("A"), 2, 120, strip.Color(0, 25, 0));
  animate(characterToMap("A"), characterToMap("B"), 3, 120, strip.Color(0, 0, 25));
  animate(characterToMap("B"), characterToMap("C"), 3, 120, strip.Color(25, 0, 0));
}

void flashlight()
{
  strip.fill(strip.Color(flashlightBrightness * 0.2 * flashlightColorR, flashlightBrightness * 0.2 * flashlightColorG, flashlightBrightness * 0.2 * flashlightColorB));
  for (int s = 0; s < 8; s++)
    strip.setPixelColor(led_map[0][s], strip.Color(flashlightBrightness * 0.4 * flashlightColorR, flashlightBrightness * 0.4 * flashlightColorG, flashlightBrightness * 0.4 * flashlightColorB));
  for (int e = 0; e < 8; e++)
    strip.setPixelColor(led_map[7][e], strip.Color(flashlightBrightness * 0.4 * flashlightColorR, flashlightBrightness * 0.4 * flashlightColorG, flashlightBrightness * 0.4 * flashlightColorB));
  for (int x = 0; x < 8; x++)
    strip.setPixelColor(led_map[x][0], strip.Color(flashlightBrightness * 0.4 * flashlightColorR, flashlightBrightness * 0.4 * flashlightColorG, flashlightBrightness * 0.4 * flashlightColorB));
  for (int y = 0; y < 8; y++)
    strip.setPixelColor(led_map[y][7], strip.Color(flashlightBrightness * 0.4 * flashlightColorR, flashlightBrightness * 0.4 * flashlightColorG, flashlightBrightness * 0.4 * flashlightColorB));
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

struct WiFiCredentials
{
  char ssid[32];
  char password[32];
};

void saveWiFiCredentials(const char *ssid, const char *password)
{
  WiFiCredentials credentials;

  strncpy(credentials.ssid, ssid, 32);
  strncpy(credentials.password, password, 32);

  EEPROM.begin(sizeof(credentials));
  EEPROM.put(0, credentials);
  EEPROM.commit();
  EEPROM.end();
}

bool loadWiFiCredentials(char *ssid, char *password)
{
  WiFiCredentials credentials;

  EEPROM.begin(sizeof(credentials));
  EEPROM.get(0, credentials);
  EEPROM.end();

  if (strlen(credentials.ssid) == 0 || strlen(credentials.password) == 0)
    return false;

  strncpy(ssid, credentials.ssid, 32);
  strncpy(password, credentials.password, 32);

  return true;
}

void wiFiInit()
{
  char savedSsid[32];
  char savedPass[32];
  if (loadWiFiCredentials(savedSsid, savedPass))
  {
    WiFi.begin(savedSsid, savedPass); // If yes, try to connect
    Serial.print("[INFO] Connecting to: ");
    Serial.println(savedSsid);
  }
  else
    Serial.print("[ERROR] No wifi info saved.");
}

//                      <--- Firmware updater --->

void firmwareUpdate() // Updater
{

  display(characters.space);
  strip.setPixelColor(led_map[0][0], LED_COLOR_UPD);
  strip.show();
  server.end();

  bool secStage = false, dualUpdate = false;

  WiFiClientSecure client; // Create secure wifi client
  client.setTrustAnchors(&cert);
  time_t now = time(nullptr);

  // Set LEDs
  strip.setPixelColor(led_map[0][0], LED_COLOR_0);
  if (updateFv && updateFS)
  {
    for (uint8_t i = 0; i < 8; i++)
      strip.setPixelColor(led_map[0][i], LED_COLOR_UPD);
    for (uint8_t i = 0; i < 8; i++)
      strip.setPixelColor(led_map[3][i], LED_COLOR_UPD);
    for (uint8_t i = 0; i < 8; i++)
      strip.setPixelColor(led_map[4][i], LED_COLOR_UPD);
    for (uint8_t i = 0; i < 8; i++)
      strip.setPixelColor(led_map[7][i], LED_COLOR_UPD);
    for (uint8_t i = 0; i < 8; i++)
      strip.setPixelColor(led_map[i][0], LED_COLOR_UPD);
    for (uint8_t i = 0; i < 8; i++)
      strip.setPixelColor(led_map[i][7], LED_COLOR_UPD);

    dualUpdate = true;
  }
  else
  {
    for (uint8_t i = 0; i < 8; i++)
      strip.setPixelColor(led_map[2][i], LED_COLOR_UPD);
    for (uint8_t i = 0; i < 8; i++)
      strip.setPixelColor(led_map[5][i], LED_COLOR_UPD);
    strip.setPixelColor(led_map[3][0], LED_COLOR_UPD);
    strip.setPixelColor(led_map[4][0], LED_COLOR_UPD);
    strip.setPixelColor(led_map[3][7], LED_COLOR_UPD);
    strip.setPixelColor(led_map[4][7], LED_COLOR_UPD);
  }
  strip.show();

  // Update progress - change leds
  ESPhttpUpdate.rebootOnUpdate(false);
  ESPhttpUpdate.onStart([]()
                        { Serial.println("Starting update..."); });

  ESPhttpUpdate.onProgress([&](int current, int total)
                           {
    const float progress = (float)current / (float)total;
    const uint8_t value = round(progress * 4) + 1;

    strip.setPixelColor(led_map[dualUpdate ? (secStage ? 5 : 1) : 3][value], LED_COLOR_CONN);
    strip.setPixelColor(led_map[dualUpdate ? (secStage ? 6 : 2) : 4][value], LED_COLOR_CONN);
    strip.show(); });

  ESPhttpUpdate.onEnd([&]()
                      {
    strip.setPixelColor(led_map[dualUpdate ? (secStage ? 5 : 1) : 3][6], LED_COLOR_CONN);
    strip.setPixelColor(led_map[dualUpdate ? (secStage ? 6 : 2) : 4][6], LED_COLOR_CONN);
    if (!updateFS) {
      File versionFile = SPIFFS.open("/version.txt", "w");
      versionFile.seek(0);
      versionFile.write(versionString, sizeof(versionString));
      Serial.println("[INFO] Updating version.txt based on http string:\n-----");
      Serial.println(versionString);
      Serial.println("-----");
      versionFile.close();
    }
    strip.show(); });

  t_httpUpdate_return ret;
  if (strlen(updateFv)) // Update firmware
    ret = ESPhttpUpdate.update(client, updateFv);

  secStage = true; // Update filesystem
  SPIFFS.end();
  if (strlen(updateFS) && (!strlen(updateFv) || ret == HTTP_UPDATE_OK || ret == 0))
    ret = ESPhttpUpdate.updateFS(client, updateFS);

  if (ret != HTTP_UPDATE_OK && ret != 0)
  { // Error
    Serial.print("[ERROR] ");
    Serial.println(ESPhttpUpdate.getLastErrorString());
    Serial.println(ret);
    strip.setPixelColor(led_map[dualUpdate ? (secStage ? 5 : 1) : 3][0], LED_COLOR_ERR);
    strip.setPixelColor(led_map[dualUpdate ? (secStage ? 6 : 2) : 4][0], LED_COLOR_ERR);

    strip.setPixelColor(led_map[dualUpdate ? (secStage ? 5 : 1) : 3][7], LED_COLOR_ERR);
    strip.setPixelColor(led_map[dualUpdate ? (secStage ? 6 : 2) : 4][7], LED_COLOR_ERR);
    strip.show();
    delay(2000);
  }
  else
  {
    delay(500);
    animate(characters.space, characters.updater, 2, 100, LED_COLOR_CONN);
    delay(500);
  }
}

// ------------------------
// ------- Server ---------
// ------------------------

void initServer()
{

  configTime(2 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  // Sites
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->redirect("/home"); });
  server.on("/home", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/html/index.html", "text/html"); });
  server.on("/settings", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/html/settings.html", "text/html"); });
  server.on("/patterns", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/html/patterns.html", "text/html"); });
  server.on("/flashlight", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/html/flashlight.html", "text/html"); });

  // Additional html & css
  server.on("/html/footer.html", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/html/footer.html", "text/html"); });
  server.on("/style/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/style/style.css", "text/css"); });
  server.on("/style/footer.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/style/footer.css", "text/css"); });

  // Javascript
  server.on("/scripts/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/scripts/script.js", "text/javascript"); });
  server.on("/scripts/flashlight.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/scripts/flashlight.js", "text/javascript"); });
  server.on("/scripts/settings.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/scripts/settings.js", "text/javascript"); });
  server.on("/scripts/patterns.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/scripts/patterns.js", "text/javascript"); });

  // Files
  server.on("/images/logo.png", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/images/logo.png", String(), true); });
  server.on("/images/blackhole.jpg", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/images/blackhole.jpg", String(), true); });
  server.on("/images/cosmos.jpg", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/images/cosmos.jpg", String(), true); });
  server.on("/images/space.jpg", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/images/space.jpg", String(), true); });
  server.on("/updater.json", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/updater.json", String(), true); });

  // Functions
  server.on("/functions/getSystemInfo", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    File file = SPIFFS.open("/version.txt", "r");  // Read versions
    String version = file.readString();
    file.close();
    String textToReturn = version + "\nChip ID: " + String(ESP.getChipId());
    request->send(200, "text/plain", textToReturn); });

  server.on("/functions/checkFlashlight", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String searchParams = "brightness=" + String(flashlightBrightness) + "&color[R]=" + String(flashlightColorR) + "&color[G]=" + String(flashlightColorG) + "&color[B]=" + String(flashlightColorB);
    request->send(200, "application/x-www-form-urlencoded", searchParams); });

  server.on(
      "/functions/LEDs/flashlight", HTTP_POST, [](AsyncWebServerRequest *request)
      {
      patternNum = 0;
      flashlightBrightness = request->getParam("brightness", true)->value().toFloat();
      if(request->hasParam("color[R]", true))
        flashlightColorR = request->getParam("color[R]", true)->value().toInt();
      if(request->hasParam("color[G]", true))
        flashlightColorG = request->getParam("color[G]", true)->value().toInt();
      if(request->hasParam("color[B]", true))
        flashlightColorB = request->getParam("color[B]", true)->value().toInt();

    request->send(200, "text/plain", "OK"); });

  server.on(
      "/functions/LEDs/changePattern", HTTP_POST, [](AsyncWebServerRequest *request)
      {

    if(request->hasParam("start", true)) {
      patternNum=-1;
      displayPatternJson.to<JsonArray>();
    }
    JsonObject obj = displayPatternJson.as<JsonArray>().createNestedObject();

    if(request->hasParam("from", true)) obj["from"] = request->getParam("from", true)->value();
    if(request->hasParam("to", true)) obj["to"] = request->getParam("to", true)->value();
    if(request->hasParam("color[R]", true)) obj["color"]["R"] = request->getParam("color[R]", true)->value().toInt();
    if(request->hasParam("color[G]", true)) obj["color"]["G"] = request->getParam("color[G]", true)->value().toInt();
    if(request->hasParam("color[B]", true)) obj["color"]["B"] = request->getParam("color[B]", true)->value().toInt();
    if(request->hasParam("animType", true)) obj["animType"] = request->getParam("animType", true)->value().toInt();
    if(request->hasParam("animSpeed", true)) obj["animSpeed"] = request->getParam("animSpeed", true)->value().toInt();
    if(request->hasParam("delay", true)) obj["delay"] = request->getParam("delay", true)->value().toInt();
    if(request->hasParam("end", true)) patternNum=1;
    
    request->send(200, "text/plain", "OK"); });

  server.on("/functions/updater/update", HTTP_POST, [](AsyncWebServerRequest *request)
            {
              if (request->hasParam("firmware", true))
                request->getParam("firmware", true)->value().toCharArray(updateFv, 150);

              if (request->hasParam("filesystem", true))
                request->getParam("filesystem", true)->value().toCharArray(updateFS, 150);

              if (request->hasParam("versions", true))
                request->getParam("versions", true)->value().toCharArray(versionString, 60); });

  server.on("/functions/connCheck", HTTP_HEAD, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", "OK"); });

  server.begin();
  serverOn = true;

  Serial.print("[INFO] Server IP: ");
  Serial.println(WiFi.localIP());
}

// ------------------------
// ----- Setup & loop -----
// ------------------------

void setup()
{
  Serial.begin(9600); // Begin serial
  Serial.println("[STATUS] Start!");

  strip.begin(); // Init strips
  display(characters.space);

  if (!SPIFFS.begin())
    ESP.restart(); // Begin filesystem

  pinMode(LED_BUILTIN, OUTPUT); // Set pin modes
  digitalWrite(LED_BUILTIN, HIGH);

  if (WiFi_UpdateCredentialsFile)
    saveWiFiCredentials(ssid, password); // Save network config

  wiFiInit(); // Connect to wifi
}

void loop()
{
  if (flashlightBrightness)
    flashlight();

  else if (!patternNum)
    mainAnimation();

  else if (patternNum == 1)
  {
    for (JsonVariant obj : displayPatternJson.as<JsonArray>())
    {
      animate(characterToMap(obj["from"].as<String>()), characterToMap(obj["to"].as<String>()), obj["animType"].as<int>(), obj["animSpeed"].as<int>(), strip.Color(obj["color"]["R"].as<int>(), obj["color"]["G"].as<int>(), obj["color"]["B"].as<int>()));
      delay(obj["delay"].as<int>());
    }
  }

  if (strlen(updateFv) || strlen(updateFS))
  {
    firmwareUpdate(); // Update firmware if server requested
    ESP.restart();    // End update
  }

  if (WiFi.status() == WL_CONNECTED && !serverOn)
  {
    initServer(); // Start server if wifi initialized
  }
}