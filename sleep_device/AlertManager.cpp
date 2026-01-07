#include "AlertManager.h"
#include <WiFi.h>
#include <HTTPClient.h>

static const char* ssid = "CMDI"; // Wifi name
static const char* password = "cmdi2020"; // Wifi password

static String server = "http://maker.ifttt.com";
static String eventName = "sleep_monitor_trigger"; // event name for IFTTT
static String IFTTT_Key = "dxqz3s1hMZV-FUHcWqTvKf"; // IFTTT key

static unsigned long lastAlertTime = 0;
static const unsigned long alertCooldown = 10000;

void Alert_Init() {

  // Initiate Wifi 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

// Allow alert to be sent after cooldown
bool Alert_CanSend() {
  return millis() - lastAlertTime > alertCooldown;
}

bool Alert_Send() {
  String url = server + "/trigger/" + eventName + "/with/key/" + IFTTT_Key; // IFTTT url

  // Starts web connection
  HTTPClient http;
  http.begin(url);
  int code = http.GET();
  http.end();

  // If failed connection
  if (code > 0) {
    lastAlertTime = millis();
    return true;
  }
  return false;
}

// Define time since last alert sent
unsigned long Alert_TimeSinceLast() {
  return millis() - lastAlertTime;
}
