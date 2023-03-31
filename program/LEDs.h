#include <Adafruit_NeoPixel.h>

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

  animate(alphabet.A, alphabet.B, 0, animLength);
  delay(gaps);
  animate(alphabet.B, alphabet.C, 0, animLength);
  delay(gaps);
  animate(alphabet.C, alphabet.D, 0, animLength);
  delay(gaps);
  animate(alphabet.D, alphabet.E, 0, animLength);
  delay(gaps);
  animate(alphabet.E, alphabet.F, 0, animLength);
  delay(gaps);
  animate(alphabet.F, alphabet.G, 0, animLength);
  delay(gaps);
  animate(alphabet.G, alphabet.H, 0, animLength);
  delay(gaps);
  animate(alphabet.H, alphabet.I, 0, animLength);
  delay(gaps);
  animate(alphabet.I, alphabet.J, 0, animLength);
  delay(gaps);
  animate(alphabet.J, alphabet.K, 0, animLength);
  delay(gaps);
  animate(alphabet.K, alphabet.L, 0, animLength);
  delay(gaps);
  animate(alphabet.L, alphabet.M, 0, animLength);
  delay(gaps);
  animate(alphabet.M, alphabet.N, 0, animLength);
  delay(gaps);
  animate(alphabet.N, alphabet.O, 0, animLength);
  delay(gaps);
  animate(alphabet.O, alphabet.P, 0, animLength);
  delay(gaps);
  animate(alphabet.P, alphabet.R, 0, animLength);
  delay(gaps);
  animate(alphabet.R, alphabet.S, 0, animLength);
  delay(gaps);
  animate(alphabet.S, alphabet.T, 0, animLength);
  delay(gaps);
  animate(alphabet.T, alphabet.U, 0, animLength);
  delay(gaps);
  animate(alphabet.U, alphabet.W, 0, animLength);
  delay(gaps);
  animate(alphabet.W, alphabet.X, 0, animLength);
  delay(gaps);
  animate(alphabet.X, alphabet.Y, 0, animLength);
  delay(gaps);
  animate(alphabet.Y, alphabet.Z, 0, animLength);
  delay(gaps ? gaps * 2 : 200);

  animate(alphabet.Z, alphabet.Y, 1, animLength);
  delay(gaps);
  animate(alphabet.Y, alphabet.X, 1, animLength);
  delay(gaps);
  animate(alphabet.X, alphabet.W, 1, animLength);
  delay(gaps);
  animate(alphabet.W, alphabet.U, 1, animLength);
  delay(gaps);
  animate(alphabet.U, alphabet.T, 1, animLength);
  delay(gaps);
  animate(alphabet.T, alphabet.S, 1, animLength);
  delay(gaps);
  animate(alphabet.S, alphabet.R, 1, animLength);
  delay(gaps);
  animate(alphabet.R, alphabet.P, 1, animLength);
  delay(gaps);
  animate(alphabet.P, alphabet.O, 1, animLength);
  delay(gaps);
  animate(alphabet.O, alphabet.N, 1, animLength);
  delay(gaps);
  animate(alphabet.N, alphabet.M, 1, animLength);
  delay(gaps);
  animate(alphabet.M, alphabet.L, 1, animLength);
  delay(gaps);
  animate(alphabet.L, alphabet.K, 1, animLength);
  delay(gaps);
  animate(alphabet.K, alphabet.J, 1, animLength);
  delay(gaps);
  animate(alphabet.J, alphabet.I, 1, animLength);
  delay(gaps);
  animate(alphabet.I, alphabet.H, 1, animLength);
  delay(gaps);
  animate(alphabet.H, alphabet.G, 1, animLength);
  delay(gaps);
  animate(alphabet.G, alphabet.F, 1, animLength);
  delay(gaps);
  animate(alphabet.F, alphabet.E, 1, animLength);
  delay(gaps);
  animate(alphabet.E, alphabet.D, 1, animLength);
  delay(gaps);
  animate(alphabet.D, alphabet.C, 1, animLength);
  delay(gaps);
  animate(alphabet.C, alphabet.B, 1, animLength);
  delay(gaps);
  animate(alphabet.B, alphabet.A, 1, animLength);
  delay(gaps ? gaps * 2 : 200);
}