#include "AlertManager.h"
#include <WiFi.h>
#include <HTTPClient.h>

static const char* ssid = ""; // fill with wifi name
static const char* password = ""; // fill with wifi password

static String server = "http://maker.ifttt.com";
static String eventName = ""; //fill with event name
static String IFTTT_Key = ""; // fill with IFTTT key

static unsigned long lastAlertTime = 0;
static const unsigned long alertCooldown = 10000;

void Alert_Init() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void Alert_Update() {
  // placeholder for future logic (reconnect, etc.)
}

bool Alert_CanSend() {
  return millis() - lastAlertTime > alertCooldown;
}

bool Alert_Send() {
  String url = server + "/trigger/" + eventName + "/with/key/" + IFTTT_Key;

  HTTPClient http;
  http.begin(url);
  int code = http.GET();
  http.end();

  if (code > 0) {
    lastAlertTime = millis();
    return true;
  }
  return false;
}

unsigned long Alert_TimeSinceLast() {
  return millis() - lastAlertTime;
}

