# ESP32 Classroom Occupancy Checker

This project uses an ESP32 microcontroller and an infrared sensor to monitor classroom occupancy and update the status to a Supabase database in real time.

## Features

- Detects room occupancy using an IR sensor.
- Connects to Wi-Fi automatically.
- Sends status updates ("Occupied"/"Not Occupied") to a Supabase Postgres database.
- Avoids redundant updates (only sends when the status changes).
- Built-in reconnection logic for Wi-Fi interruptions.
- Serial monitor output for debugging.

## How it Works

1. The ESP32 reads the IR sensor (connected to pin D32).
2. If the sensor detects presence, the room is marked as "Occupied"; otherwise, it's "Not Occupied".
3. When the occupancy status changes, the ESP32 sends a PATCH request to Supabase, updating the status of room 1 in the "rooms" table.
4. All API communication uses the official REST interface with appropriate headers and Supabase API keys.

## Wiring

| ESP32 Pin | Component         |
|-----------|-------------------|
| 32 (D32)  | IR Sensor Output  |
| 3V3/GND   | IR Sensor VCC/GND |

## Code Overview

- Wi-Fi credentials and Supabase credentials are set via defines at the top of the code.
- The IR sensor pin is defined as 32.
- The device checks the sensor status every 500 ms.
- The function `sendStatusToSupabase()` is responsible for preparing the JSON payload and making the HTTP PATCH request to Supabase.

## Usage

1. Update your Wi-Fi SSID and PASSWORD in the code.
2. Update your Supabase URL, API key, and table name if needed.
3. Flash the code to your ESP32.
4. Open Serial Monitor at 115200 baud to view logs.
5. The ESP32 will connect to Wi-Fi, read the sensor, and update the Supabase database whenever occupancy status changes.

## Dependencies

- [WiFi.h](https://www.arduino.cc/en/Reference/WiFi)
- [HTTPClient.h](https://www.arduino.cc/en/Reference/HTTPClient)
- [ArduinoJson.h](https://arduinojson.org/)

## Supabase Table Example

You need a table called rooms with at least these columns:

| Column       | Type    |
|--------------|---------|
| room_number  | int     |
| status       | text    |
