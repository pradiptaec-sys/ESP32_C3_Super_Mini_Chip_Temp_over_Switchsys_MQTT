# ESP32-C3 Super Mini — Chip Temperature over MQTT

A lightweight Arduino sketch for the **ESP32-C3 Super Mini** that reads the chip's internal temperature sensor and publishes it as JSON to an MQTT broker over Wi-Fi.

## Features

- Connects to Wi-Fi and an MQTT broker
- Publishes internal chip temperature as a JSON payload
- Non-blocking Wi-Fi/MQTT reconnect handling using `millis()` (no `delay()` in the connection logic)
- Small MQTT buffer footprint (256 bytes)

## Hardware

- ESP32-C3 Super Mini board
- Wi-Fi network with internet/LAN access to the MQTT broker

## Dependencies

Install via the Arduino Library Manager:

- [`PubSubClient`](https://github.com/knolleary/pubsubclient)
- ESP32 board support package (`WiFi.h`, FreeRTOS headers are bundled with the ESP32 Arduino core)

## Configuration

Edit the `USER CONFIG` section at the top of the `.ino` file before flashing:

```cpp
const char* ssid = "Switchsys";
const char* password = "11223344";

const char* mqtt_server = "mqtt.switchsys.in";
const int mqtt_port = 1883;

const char* chip_temp_topic = "switchsysmqtt/esp32c3/chip_temp";
```

> ⚠️ **Security note:** Wi-Fi credentials and the MQTT broker address are hardcoded in plain text. Avoid committing your real credentials to a public repository — consider moving them to a `secrets.h` file that's excluded via `.gitignore`, or using environment-based config for production deployments.

## MQTT Payload

Published to `chip_temp_topic` as JSON:

```json
{"chip_temp":42.5}
```

- Temperature is in °C, reported to 1 decimal place.
- The value comes from the ESP32's internal die temperature sensor (`temperatureRead()`), which reflects **chip temperature**, not ambient temperature.

## Behavior

- **Wi-Fi:** reconnects automatically (blocking) if the connection drops.
- **MQTT:** non-blocking reconnect attempted every 3 seconds if disconnected.
- **Publishing:** intended interval is set by `PUBLISH_INTERVAL_MS` (default `1000` ms), but a `delay(10000)` after each publish call currently blocks the loop for 10 seconds, making the **effective publish interval ~10 seconds** rather than 1. Remove or reduce that `delay()` if you want true 1-second publishing.

## Getting Started

1. Open the `.ino` file in the Arduino IDE (or PlatformIO).
2. Install the ESP32 board package and the `PubSubClient` library.
3. Update the Wi-Fi and MQTT settings in the `USER CONFIG` section.
4. Select **ESP32C3 Dev Module** (or your specific board variant) as the target board.
5. Flash the sketch and open the Serial Monitor at `115200` baud to view connection logs and published payloads.

## License

Add a license of your choice (e.g., MIT) here.
