#include "server.h"

void cba() {
  animate(alphabet.C, alphabet.B, 2, 120, strip.Color(0, 0, 25));
  animate(alphabet.B, alphabet.A, 2, 120, strip.Color(0, 25, 0));
  animate(alphabet.A, alphabet.B, 3, 120, strip.Color(0, 0, 25));
  animate(alphabet.B, alphabet.C, 3, 120, strip.Color(25, 0, 0));
}

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

  if (updateFirmware) {
    delay(1000);
    firmwareUpdate();  // Update firmware if server requested
    updateFirmware = false;
  }

  if (WiFi.status() == WL_CONNECTED && !serverOn) initServer();  // Start server if wifi initialized
}