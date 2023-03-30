#include "updater.h"
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebSrv.h>

AsyncWebServer server(80);  // utworzenie serwera na porcie 80

uint8_t patternNum = 0;
bool serverOn = false, updateFirmware = false;

void initServer() {

  // Home site and reuired additional htmls, csss and jss

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(SPIFFS, "/index.html", String(), false);
  });
  server.on("/html/footer.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/html/footer.html","text/html");
  });
  server.on("/style/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style/style.css","text/css");
  });
  server.on("/style/footer.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style/footer.css","text/css");
  });
  server.on("/scripts/loading.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/scripts/loading.js","text/js");
  });
  server.on("/scripts/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/scripts/script.js","text/js");
  });

  // Auto-refresh animation pattern
  server.on("/getledspattern", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(patternNum));
  });


  // Change pattern
  server.on("/change", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("Received /change");
    patternNum++;
    if (patternNum > 1) patternNum = 0;
    request->redirect("/");
  });

  // Updater
  server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request) {
    updateFirmware = true;
    request->redirect("/");
  });

  server.begin();

  Serial.print("[INFO] Server IP: ");
  Serial.println(WiFi.localIP());

  serverOn = true;
}