#include "updater.h"

void cba() {
  animate(alphabet.C, alphabet.B, 2, 120, strip.Color(0, 0, 20));
  animate(alphabet.B, alphabet.A, 2, 120, strip.Color(0, 20, 0));
  animate(alphabet.A, alphabet.B, 3, 120, strip.Color(0, 0, 20));
  animate(alphabet.B, alphabet.C, 3, 120, strip.Color(20, 0, 0));
}

void setup() {
  Serial.begin(9600);  // Begin serial
  Serial.println("[STATUS] Start!");

  if (!SPIFFS.begin()) ESP.restart();              // Begin filesystem 
    
  pinMode(LED_BUILTIN, OUTPUT);  // Set pin modes
  digitalWrite(LED_BUILTIN, LOW);

  display(alphabet.W);

  if (WiFi_UpdateCredentialsFile) saveWifiCfg();  // Save network config
  wiFiInit();  // Connect to wifi
  firmwareUpdate();  
  
  strip.begin();    // Init strips
  strip.show();

  display(alphabet.C);
  delay(1000);
}

void loop() {
  // Here paste the functions you like to use
  alphabetAnim();
}