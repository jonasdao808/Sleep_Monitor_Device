#include <Arduino.h>
#include "AlertManager.h"
#include "system_state.h"

#define SPO2_LIMIT 92
#define ALERT_COOLDOWN 10000

void AlertTask(void *pvParameters) {
  while (true) {
    unsigned long currentTime = millis();

    xSemaphoreTake(sysMutex, portMAX_DELAY);
    if (sys.snoreDetected &&
        sys.validSpO2 &&
        sys.spo2 < SPO2_LIMIT &&
        (currentTime - sys.lastAlertTime) > ALERT_COOLDOWN) {

      sys.alertRequested = true;
      sys.lastAlertTime = currentTime;
    }
    xSemaphoreGive(sysMutex);

    vTaskDelay(pdMS_TO_TICKS(300));
  }
}
