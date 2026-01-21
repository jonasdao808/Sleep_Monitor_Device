#include <Arduino.h>
#include "SerialOutput.h"
#include "system_state.h"

void SerialTask(void *pvParameters) {
  while (true) {
    xSemaphoreTake(sysMutex, portMAX_DELAY);

    Serial.print("RMS: "); Serial.print(sys.rms);
    Serial.print(" | HR: "); Serial.print(sys.validHR ? sys.heartRate : -1);
    Serial.print(" | SpO2: "); Serial.print(sys.validSpO2 ? sys.spo2 : -1);
    Serial.print(" | Temp: "); Serial.print(sys.temperatureC, 2);
    Serial.print(" | AlertPending: "); Serial.println(sys.alertRequested);

    xSemaphoreGive(sysMutex);

    vTaskDelay(pdMS_TO_TICKS(200));
  }
}
