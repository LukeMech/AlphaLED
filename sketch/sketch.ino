#include "updater.h"

void ICACHE_FLASH_ATTR cba() {
  animate(alphabet.C, alphabet.B, 2, 120, strip.Color(0, 0, 20));
  animate(alphabet.B, alphabet.A, 2, 120, strip.Color(0, 20, 0));
  animate(alphabet.A, alphabet.B, 3, 120, strip.Color(0, 0, 20));
  animate(alphabet.B, alphabet.C, 3, 120, strip.Color(20, 0, 0));
}

bool updaterUpdate;
void setup() {
  Serial.begin(9600);  // Begin serial
  Serial.println("[STATUS] Start!");

  strip.begin();    // Init strips
  strip.show();

  if (!SPIFFS.begin()) ESP.restart();              // Begin filesystem 
    
  pinMode(LED_BUILTIN, OUTPUT);  // Set pin modes
  digitalWrite(LED_BUILTIN, HIGH);

  if (WiFi_UpdateCredentialsFile) saveWifiCfg();  // Save network config
  system_os_task(wiFiInit, USER_TASK_PRIO_0, connectToWifiEventQueue, connectToWifiEventQueueLen);
  system_os_post(USER_TASK_PRIO_0, 0, 0);
  system_os_task(firmwareUpdate, USER_TASK_PRIO_1, firmwareDownloadEventQueue, firmwareDownloadEventQueueLen);
  system_os_post(USER_TASK_PRIO_1, 0, 0);
}

void loop() {

  // Here paste the functions you like to use
  alphabetAnim();
}