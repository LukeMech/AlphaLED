#include <Adafruit_NeoPixel.h>

// <-- Settings - required -->

#define LED_PIN D1  // LED Pin

#define LED_COLOR_0 strip.Color(0, 0, 0)    // diode color for 0 (background) (R G B)
#define LED_COLOR_1 strip.Color(20, 20, 0)  // diode color for 1 (text) (R G B)

uint8_t led_map[8][8] = {  // Table corresponding to the physical position/number of the LEDs
  { 63, 62, 61, 60, 59, 58, 57, 56 },
  { 48, 49, 50, 51, 52, 53, 54, 55 },
  { 47, 46, 45, 44, 43, 42, 41, 40 },
  { 32, 33, 34, 35, 36, 37, 38, 39 },
  { 31, 30, 29, 28, 27, 26, 25, 24 },
  { 16, 17, 18, 19, 20, 21, 22, 23 },
  { 15, 14, 13, 12, 11, 10, 9, 8 },
  { 0, 1, 2, 3, 4, 5, 6, 7 }
};


// <-- To use out-of-the-box -->

// Alphabet maps
struct Maps {
  uint8_t
    A[8][8] = { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 1, 1, 1, 1, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    B[8][8] = { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 1, 1, 1, 0, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 1, 1, 1, 0, 0, 0 }, { 0, 0, 1, 1, 1, 0, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 1, 1, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    C[8][8] = { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    D[8][8] = { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 1, 1, 1, 0, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 1, 1, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    E[8][8] = { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 1, 1, 1, 1, 0, 0 }, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 0, 0, 1, 1, 1, 0, 0, 0 }, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 0, 0, 1, 1, 1, 1, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    F[8][8] = { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 1, 1, 1, 1, 0, 0 }, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 0, 0, 1, 1, 1, 0, 0, 0 }, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    G[8][8] = { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 0, 0, 1, 0, 1, 1, 0, 0 }, { 0, 0, 1, 0, 0, 1, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    H[8][8] = {
      { 0, 0, 0, 0, 0, 0, 0, 0 },
      { 0, 0, 1, 0, 0, 1, 0, 0 },
      { 0, 0, 1, 0, 0, 1, 0, 0 },
      { 0, 0, 1, 1, 1, 1, 0, 0 },
      { 0, 0, 1, 1, 1, 1, 0, 0 },
      { 0, 0, 1, 0, 0, 1, 0, 0 },
      { 0, 0, 1, 0, 0, 1, 0, 0 },
      { 0, 0, 0, 0, 0, 0, 0, 0 },
    };

} alphabet;

// Init LEDs
Adafruit_NeoPixel strip(64, LED_PIN, NEO_GRB + NEO_KHZ800);

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
void animate(uint8_t startMap[][8], uint8_t endMap[][8], uint8_t direction = 0, uint8_t gap = 100) {
  // Direction 0: Text ---->>>
  if (!direction) {
    for (uint8_t shift = 1; shift < 7; shift++) {
      for (uint8_t y = 0; y < 8; y++) {
        for (uint8_t x = 0; x < 8; x++) {
          uint8_t value = 0;
          if (x < (6 - shift)) value = startMap[y][x + shift];
          else value = endMap[y][x - (6 - shift)];
          uint8_t index = led_map[y][x];
          if (value == 0) strip.setPixelColor(index, LED_COLOR_0);
          else if (value == 1) strip.setPixelColor(index, LED_COLOR_1);
        }
      }
      strip.show();
      delay(gap);
    }
  }
  // Direction 1: <<<---- Text
  else {
    for (int8_t shift = 5; shift >= 0; shift--) {
      for (uint8_t y = 0; y < 8; y++) {
        for (uint8_t x = 0; x < 8; x++) {
          uint8_t value = 0;
          if (x >= (6 - shift)) value = startMap[y][x - (6 - shift)];
          else value = endMap[y][x + shift];
          uint8_t index = led_map[y][x];
          if (value == 0) strip.setPixelColor(index, LED_COLOR_0);
          else if (value == 1) strip.setPixelColor(index, LED_COLOR_1);
        }
      }
      strip.show();
      delay(gap);
    }
  }
}

void setup() {
  strip.begin();
  strip.show();
}


void loop() {

  // Here paste the functions you like to use

  int del = 500;

  animate(alphabet.A, alphabet.B);
  delay(del);
  animate(alphabet.B, alphabet.C);
  delay(del);
  animate(alphabet.C, alphabet.D);
  delay(del);
  animate(alphabet.D, alphabet.E);
  delay(del);
  animate(alphabet.E, alphabet.F);
  delay(del);
  animate(alphabet.F, alphabet.G);
  delay(del);
  animate(alphabet.G, alphabet.H);
  delay(del);

  animate(alphabet.H, alphabet.G, 1);
  delay(del);
  animate(alphabet.G, alphabet.F, 1);
  delay(del);
  animate(alphabet.F, alphabet.E, 1);
  delay(del);
  animate(alphabet.E, alphabet.D, 1);
  delay(del);
  animate(alphabet.D, alphabet.C, 1);
  delay(del);
  animate(alphabet.C, alphabet.B, 1);
  delay(del);
  animate(alphabet.B, alphabet.A, 1);
  delay(del);
}