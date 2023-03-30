#include "updater.h"
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebSrv.h>

AsyncWebServer server(80);  // utworzenie serwera na porcie 80

uint8_t patternNum = 0;
bool serverOn = false, updateFirmware = false;

void initServer() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    File file = SPIFFS.open("/index.html", "r");
    if (file) {
      request->send(200, "text/html", file.readString());
    } else {
      request->send(404, "text/plain", "File index.html not found");
      updateFirmware = true;
    }
    file.close();
  });

  server.on("/change", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("Received /change");
    patternNum++;
    if (patternNum > 1) patternNum = 0;

    File file = SPIFFS.open("/index.html", "r");
    if (file) {
      request->send(200, "text/html", file.readString());
    } else request->send(200, "text/plain", "Changing animation style, wait for current animation to complete!");
    file.close();
  });

  server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request) {
    updateFirmware = true;

    File file = SPIFFS.open("/index.html", "r");
    if (file) {
      request->send(200, "text/html", file.readString());
    } else request->send(200, "text/plain", "Calling for updater right after animation is done!");
    file.close();
  });

  server.on("/getledspattern", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(patternNum));
  });

  server.begin();

  Serial.print("[INFO] Server IP: ");
  Serial.println(WiFi.localIP());

  serverOn = true;
}