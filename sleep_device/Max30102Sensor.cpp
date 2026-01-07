#include "Max30102Sensor.h"
#include <Arduino.h>
#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"

// Defining SDA and SCL since defaults are in use by mic
#define I2C_SDA 18
#define I2C_SCL 19
#define BUFFER_LENGTH 100

static MAX30105 particleSensor;

static uint32_t irBuffer[BUFFER_LENGTH]; // infared LED sensor data
static uint32_t redBuffer[BUFFER_LENGTH]; //red LED sensor data

static int32_t spo2; // SPO2 value
static int8_t validSPO2; //indicator to show if the SPO2 calculation is valid
static int32_t heartRate; // heart rate value
static int8_t validHeartRate; //indicator to show if the heart rate calculation is valid

static uint8_t sampleIndex = 0;
static bool bufferFull = false;

void Max_Init() {
  Wire.begin(I2C_SDA, I2C_SCL);

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) { // default I2C port, 400kHz
    Serial.println("MAX30102 not found");
    while (1);
  }

  particleSensor.setup(60, 4, 2, 100, 411, 4096); // (powerLevel, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange)
  particleSensor.enableDIETEMPRDY(); // Necessary for sesnsor functionality
}

void Max_Update() {
  particleSensor.check();

  if (particleSensor.available()) {
    redBuffer[sampleIndex] = particleSensor.getRed();
    irBuffer[sampleIndex] = particleSensor.getIR();
    particleSensor.nextSample(); // Moving to next sample

    sampleIndex++;
    if (sampleIndex >= BUFFER_LENGTH) {
      sampleIndex = 0;
      bufferFull = true;
    }
  }

  // Calculating HR and SP02
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
