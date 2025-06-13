#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Wi-Fi credentials
const char* SSID = "SSID";
const char* PASSWORD = "PASSWORD";

// Supabase credentials
const char* supabaseUrl = "";
const char* supabaseKey = "";
const char* tableName = "rooms";

// Use GPIO4 instead of GPIO2
const int presencePin = 4;
String lastStatus = "";

void setup() {
  Serial.begin(115200);
  pinMode(presencePin, INPUT);

  WiFi.begin(SSID, PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi\n");
}

void loop() {
  int presenceState = digitalRead(presencePin);
  String statusToSend;

  if (presenceState == HIGH) {
    Serial.println("Presence detected");
    statusToSend = "Occupied";
  } else {
    Serial.println("No presence detected");
    statusToSend = "Not Occupied";
  }

  if (statusToSend != lastStatus) {
    sendStatusToSupabase(statusToSend);
    lastStatus = statusToSend;
  }

  delay(1000);
}

void sendStatusToSupabase(String status) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected. Attempting to reconnect...");
    WiFi.reconnect();
    int retries = 0;
    while (WiFi.status() != WL_CONNECTED && retries < 10) {
      delay(1000);
      Serial.print(".");
      retries++;
    }
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Reconnect failed.");
      return;
    }
    Serial.println("WiFi reconnected.");
  }

  StaticJsonDocument<200> jsonDoc;
  jsonDoc["status"] = status;

  String jsonString;
  serializeJson(jsonDoc, jsonString);

  HTTPClient http;
  String endpoint = String(supabaseUrl) + "/rest/v1/" + tableName + "?room_number=eq.1";
  http.begin(endpoint);

  http.addHeader("Content-Type", "application/json");
  http.addHeader("apikey", supabaseKey);
  http.addHeader("Authorization", "Bearer " + String(supabaseKey));
  http.addHeader("Prefer", "return=representation");

  int httpResponseCode = http.PATCH(jsonString);
  if (httpResponseCode > 0) {
    Serial.print("Supabase update: ");
    Serial.println(status);
    Serial.println("HTTP Response code: " + String(httpResponseCode));
    String response = http.getString();
    Serial.println("Response: " + response);
  } else {
    Serial.println("Error in HTTP request");
    Serial.println("HTTP Response code: " + String(httpResponseCode));
    String response = http.getString();
    Serial.println("Response: " + response);
  }
  http.end();
}
