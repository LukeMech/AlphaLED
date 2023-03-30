#include "LEDs.h"
// ------------------------
// -------- WiFi ----------
// ------------------------

const bool WiFi_UpdateCredentialsFile = false;  // Update network_config.txt in filesystem?
const char* ssid = "";                          // Network name
const char* password = "";                      // Network password

// ------------------------
// ------- Updater --------
// ------------------------

const char* host = "github.com";  // Host to check connection, leave as is if using github
const int httpsPort = 443;        // Host port

const char* updaterVersionCtrlUrl = "https://raw.githubusercontent.com/LukeMech/AlphaLED/main/updater/build-version.txt";  // Link to version.txt
const char* updaterFirmwareUrl = "https://raw.githubusercontent.com/LukeMech/AlphaLED/main/updater/firmware.bin";          // File to firmware.bin
const char* updaterFSUrl = "https://raw.githubusercontent.com/LukeMech/AlphaLED/main/updater/filesystem.bin";              // File to filesystem.bin

// Wifi and certificates
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

// Time for certificate validation
#include <time.h>

//HTTP
#include <ESP8266HTTPClient.h>

// Updater
#include <ESP8266httpUpdate.h>

// Filesystems
#include "FS.h"
#include <EEPROM.h>

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

  configTime(0, 0, "pool.ntp.org", "time.nist.gov");  // Set time via NTP, as required for x.509 validation
  time_t now = time(nullptr);

  if (!client.connect(host, httpsPort)) return;  // Connect to github

  HTTPClient http;  // Connect to release API
  http.begin(client, updaterVersionCtrlUrl);
  int httpCode = http.GET();
  if (httpCode != HTTP_CODE_OK) return;

  String new_version = http.getString();  //Download version tag
  int firmware_pos = new_version.indexOf("Firmware: ") + 10;
  String newFirmwareVer = new_version.substring(firmware_pos);
  int server_pos = new_version.indexOf("Server: ") + 8;
  int server_end_pos = new_version.indexOf("\nFirmware:");
  String newFsVer = new_version.substring(server_pos, server_end_pos);
  newFsVer.trim();
  newFirmwareVer.trim();
  http.end();

  String firmwareVer, fsVer;
  File file = SPIFFS.open("/version.txt", "r");  // Read versions
  while (file.available()) {
    String line = file.readStringUntil('\n');
    if (line.startsWith("Server:")) fsVer = line.substring(line.indexOf(":") + 2);
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
    strip.show();

    // Change version file if FS not updating
    if (!dualUpdate && !secStage) {
      File file = SPIFFS.open("/version.txt", "r");
      if (file) {
        String fileContent = file.readString();
        file.close();
        int firmwareIndex = fileContent.indexOf("Firmware: ") + 10;
        int firmwareEndIndex = fileContent.indexOf("\n", firmwareIndex);
        fileContent.replace(fileContent.substring(firmwareIndex, firmwareEndIndex), newFirmwareVer);
        SPIFFS.remove("/version.txt");
        file = SPIFFS.open("/version.txt", "w");
        if (file) {
          file.print("");
          file.print(fileContent);
          file.close();
        }
      }
    }
  });

  SPIFFS.end();

  t_httpUpdate_return ret;
  if (updateFirmware) ret = ESPhttpUpdate.update(client, updaterFirmwareUrl);  // Update firmware
  secStage = true;
  if ((ret == HTTP_UPDATE_OK || ret == 0 || ret==-1) && updateFS) ret = ESPhttpUpdate.updateFS(client, updaterFSUrl);  // Update filesystem

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