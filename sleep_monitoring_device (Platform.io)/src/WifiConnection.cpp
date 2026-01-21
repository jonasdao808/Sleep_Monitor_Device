#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "WifiConnection.h"
#include "system_state.h"

const char *ssid = "CMDI";
const char *password = "cmdi2020";

void sendAlert() {
  HTTPClient http;
  http.begin("http://maker.ifttt.com/trigger/sleep_monitor_trigger/with/key/YOUR_KEY");
  http.GET();
  http.end();
}

void WifiTask(void *pvParameters) {
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    vTaskDelay(pdMS_TO_TICKS(500));
  }

  while (true) {
    bool send = false;

    xSemaphoreTake(sysMutex, portMAX_DELAY);
    if (sys.alertRequested) {
      sys.alertRequested = false;
      send = true;
    }
    xSemaphoreGive(sysMutex);

    if (send) sendAlert();

    vTaskDelay(pdMS_TO_TICKS(200));
  }
}
