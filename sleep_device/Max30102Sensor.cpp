#include "Max30102Sensor.h"
#include <Arduino.h>
#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"

#define I2C_SDA 18
#define I2C_SCL 19
#define BUFFER_LENGTH 100

static MAX30105 particleSensor;

static uint32_t irBuffer[BUFFER_LENGTH];
static uint32_t redBuffer[BUFFER_LENGTH];

static int32_t spo2;
static int8_t validSPO2;
static int32_t heartRate;
static int8_t validHeartRate;

static uint8_t sampleIndex = 0;
static bool bufferFull = false;

void Max_Init() {
  Wire.begin(I2C_SDA, I2C_SCL);

  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30102 not found");
    while (1);
  }

  particleSensor.setup(60, 4, 2, 100, 411, 4096);
  particleSensor.enableDIETEMPRDY();
}

void Max_Update() {
  particleSensor.check();

  if (particleSensor.available()) {
    redBuffer[sampleIndex] = particleSensor.getRed();
    irBuffer[sampleIndex] = particleSensor.getIR();
    particleSensor.nextSample();

    sampleIndex++;
    if (sampleIndex >= BUFFER_LENGTH) {
      sampleIndex = 0;
      bufferFull = true;
    }
  }

  if (bufferFull) {
    maxim_heart_rate_and_oxygen_saturation(
      irBuffer, BUFFER_LENGTH, redBuffer,
      &spo2, &validSPO2, &heartRate, &validHeartRate
    );
  }
}

int Max_GetHeartRate() { return heartRate; }
int Max_GetSpO2() { return spo2; }
float Max_GetTemperature() { return particleSensor.readTemperature(); }

bool Max_HRValid() { return validHeartRate; }
bool Max_SpO2Valid() { return validSPO2; }
