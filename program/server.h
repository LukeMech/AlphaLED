#include "updater.h"
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebSrv.h>

AsyncWebServer server(80);  // utworzenie serwera na porcie 80

uint8_t patternNum = 0;
bool serverOn = false, updateFirmware = false;

String html = "<html><body><form method='POST' action='/change'><input type='submit' value='Change'></form><form method='POST' action='/update'><input type='submit' value='Update'></form></body></html> ";


void initServer() {
  File file = SPIFFS.open("/index.html", "r");
  if (!file) updateFirmware = true;  // Re-download filesystem

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    File file = SPIFFS.open("/index.html", "r");
    if (file) {
      request->send(200, "text/html", file.readString());
      file.close();
    } else request->send(404, "text/plain", "File index.html not found");
  });

  server.on("/change", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("Received /change");
    patternNum++;
    if (patternNum > 1) patternNum = 0;

    File file = SPIFFS.open("/index.html", "r");
    if (file) {
      request->send(200, "text/html", file.readString());
      file.close();
    } else request->send(200, "text/plain", "Changing animation style, wait for current animation to complete!");
  });

  server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request) {
    updateFirmware = true;

    File file = SPIFFS.open("/index.html", "r");
    if (file) {
      request->send(200, "text/html", file.readString());
      file.close();
    } else request->send(200, "text/plain", "Calling for updater right after animation is done!");
  });

  server.begin();
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  serverOn = true;
}