#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Wi-Fi credentials
#define SSID               "SSID_NAME"
#define PASSWORD           "SSID_PASSWORD"

// Supabase credentials
#define supabaseUrl        ""
#define supabaseKey        ""
#define tableName          "rooms"

// Infrared sensor pin (D32)
#define IR_SENSOR_PIN      32

// For tracking last status to avoid redundant updates
String lastStatus = "";

void setup() {
  Serial.begin(115200);
  pinMode(IR_SENSOR_PIN, INPUT);

  // Connect to Wi-Fi
  WiFi.begin(SSID, PASSWORD);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi\n");
}

void loop() {
  // Read sensor (HIGH = detected, LOW = nothing, but check your sensor's logic!)
  int sensorValue = digitalRead(IR_SENSOR_PIN);
  String statusToSend;

  // Adjust logic if your sensor outputs LOW for detect!
  if (sensorValue == HIGH) {
    statusToSend = "Not Occupied";
  } else {
    statusToSend = "Occupied";
  }

  // Only update if status has changed
  if (statusToSend != lastStatus) {
    sendStatusToSupabase(statusToSend);
    lastStatus = statusToSend;
  }

  delay(500); // Check sensor twice per second
}

void sendStatusToSupabase(String status) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected. Attempting to reconnect...");
    while (!WiFi.reconnect()) {
      Serial.println("Reconnecting to WiFi...");
      delay(500);
    }
    Serial.println("WiFi reconnected.");
  }

  // Prepare JSON
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
    Serial.print("Set room 1 status to: ");
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
