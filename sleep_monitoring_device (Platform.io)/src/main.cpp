#include <Arduino.h>
#include "system_state.h"

#include "mic.h"
#include "Max30102Sensor.h"
#include "AlertManager.h"
#include "WifiConnection.h"
#include "SerialOutput.h"

SystemState sys;
SemaphoreHandle_t sysMutex;

void setup() {
  Serial.begin(115200);

  sysMutex = xSemaphoreCreateMutex();

  xTaskCreatePinnedToCore(MicTask, "MicTask", 4096, NULL, 3, NULL, 1);
  xTaskCreatePinnedToCore(Max30102Task, "Max30102Task", 4096, NULL, 2, NULL, 1);
  xTaskCreatePinnedToCore(AlertTask, "AlertTask", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(WifiTask, "WifiTask", 8192, NULL, 0, NULL, 0);
  xTaskCreatePinnedToCore(SerialTask, "SerialTask", 4096, NULL, 0, NULL, 0);
}

void loop() {
  vTaskDelay(portMAX_DELAY);
}
