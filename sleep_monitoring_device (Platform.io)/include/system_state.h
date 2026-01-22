#pragma once
#include <Arduino.h>

typedef struct {
  float rms;
  bool snoreDetected;

  int heartRate;
  bool validHR;

  int spo2;
  bool validSpO2;

  float temperatureC;

  bool alertRequested;
  unsigned long lastAlertTime;
} SystemState;

extern SystemState sys;
extern SemaphoreHandle_t sysMutex;
