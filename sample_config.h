/*
 * Config settings - copy to new file called "config.h" and edit
 */
// Wifi settings
#define WIFI_SSID "my-wifi"
#define WIFI_PASS "My Password"

// broker IP/host
#define BROKER "10.0.0.1"
// broker password or blank
#define BROKER_PASS ""
// base topic to publish on (will append "/<wifi MAC id>/temp")
#define TOPIC_ROOT "Devices"
// override default timeout from PubSubClient lib
#define MQTT_KEEPALIVE 120

//Pin to use for onewire device, don't forget 4.7k resistor from +5v to data
#define ONE_WIRE_BUS D4
// How long to sleep between reads (in milliseconds) - Short values seem to really warm up the DS18B20 sensor
#define DELAY 10000
