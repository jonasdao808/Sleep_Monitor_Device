#include <Arduino.h>

#include "mic.h"
#include "Max30102Sensor.h"
#include "AlertManager.h"

#define SNORE_THRESHOLD 130
#define SPO2_THRESHOLD 92

void setup() {
  Serial.begin(115200);

  Mic_Init();
  Max_Init();
  Alert_Init();

  Serial.println("Sleep Monitor Started");
}

void loop() {
  float rms = Mic_ProcessAndGetRMS();

  Max_Update();
  Alert_Update();

  bool snoreDetected = rms > SNORE_THRESHOLD;
  bool spo2Low = Max_SpO2Valid() && Max_GetSpO2() < SPO2_THRESHOLD;
  bool canAlert = Alert_CanSend();

  bool alertSent = false;
  if (snoreDetected && spo2Low && canAlert) {
    alertSent = Alert_Send();
  }

  Serial.print("RMS: "); Serial.print((int)rms);
  Serial.print(" | HR: "); 
  Max_HRValid() ? Serial.print(Max_GetHeartRate()) : Serial.print("N/A");

  Serial.print(" | SpO2: ");
  Max_SpO2Valid() ? Serial.print(Max_GetSpO2()) : Serial.print("N/A");

  Serial.print(" | Temp: "); Serial.print(Max_GetTemperature(), 2); Serial.print(" C");

  Serial.print(" | Alert Sent: "); Serial.print(alertSent ? "YES" : "NO");
  Serial.print(" | Time Since Alert: "); 
  Serial.print(Alert_TimeSinceLast() / 1000.0); Serial.print("s");

  Serial.print(" | Can Alert: "); Serial.println(canAlert ? "YES" : "NO");

  delay(200);
}
