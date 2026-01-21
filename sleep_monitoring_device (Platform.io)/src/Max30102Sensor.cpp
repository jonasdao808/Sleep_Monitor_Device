#include <Arduino.h>
#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"
#include "Max30102Sensor.h"
#include "system_state.h"

#define SDA_PIN 18
#define SCL_PIN 19
#define BUFFER_LENGTH 100

MAX30105 sensor;
uint32_t irBuffer[BUFFER_LENGTH];
uint32_t redBuffer[BUFFER_LENGTH];

void Max30102Task(void *pvParameters) {
  Wire.begin(SDA_PIN, SCL_PIN);
  sensor.begin(Wire, I2C_SPEED_FAST);
  sensor.setup(60, 4, 2, 100, 411, 4096);
  sensor.enableDIETEMPRDY();

  int index = 0;
  bool full = false;

  while (true) {
    sensor.check();

    if (sensor.available()) {
      redBuffer[index] = sensor.getRed();
      irBuffer[index] = sensor.getIR();
      sensor.nextSample();

      if (++index >= BUFFER_LENGTH) {
        index = 0;
        full = true;
      }
    }

    if (full) {
      int32_t hr, spo2;
      int8_t vhr, vspo2;

      maxim_heart_rate_and_oxygen_saturation(
        irBuffer, BUFFER_LENGTH,
        redBuffer,
        &spo2, &vspo2,
        &hr, &vhr
      );

      float temp = sensor.readTemperature();

      xSemaphoreTake(sysMutex, portMAX_DELAY);
      sys.heartRate = hr;
      sys.validHR = vhr;
      sys.spo2 = spo2;
      sys.validSpO2 = vspo2;
      sys.temperatureC = temp;
      xSemaphoreGive(sysMutex);
    }

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}
