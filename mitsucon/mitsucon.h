// Home Assistant Mitsubishi Electric Heat Pump Controller https://github.com/unixko/MitsuCon
// using native MQTT Climate (HVAC) component with MQTT discovery for automatic configuration

// Credentials live in secrets.h (not committed), copy secrets.example.h to start
#include "secrets.h"

// Unit number, selects hostname/MQTT topic "heatpump<HP_ID>"
// Build with: --build-property "build.extra_flags=-DHP_ID=1"
#ifndef HP_ID
  #define HP_ID 0
#endif

#define _STR(x) #x
#define STR(x) _STR(x)
#define HP_NAME "heatpump" STR(HP_ID)

// enable extra MQTT topic for debug/timer info
bool _debugMode  = false;
bool _timersAttr = false;

// comment out to disable OTA
#define OTA
const char* ota_password  = OTA_PASSWORD; // also the web firmware upload password (user "admin")

// default wifi/mqtt settings, can be changed from the web interface (stored in EEPROM)
const char* cfg_default_ssid          = WIFI_SSID;
const char* cfg_default_password      = WIFI_PASSWORD;
const char* cfg_default_mqtt_server   = MQTT_SERVER;
const int cfg_default_mqtt_port       = 1883;
const char* cfg_default_mqtt_username = MQTT_USER;
const char* cfg_default_mqtt_password = MQTT_PASSWORD;

// mqtt client settings
const char* name                                = "Heat Pump " STR(HP_ID); // Device Name displayed in Home Assistant
const char* client_id                           = HP_NAME; // WiFi hostname, OTA hostname, MQTT hostname
const char* heatpump_topic                      = HP_NAME; // MQTT topic, must be unique between heat pump unit
const char* heatpump_availability_topic         = HP_NAME "/tele/avty";
const char* heatpump_state_topic                = HP_NAME "/tele/stat";
const char* heatpump_current_topic              = HP_NAME "/tele/curr";
const char* heatpump_attribute_topic            = HP_NAME "/tele/attr";
const char* heatpump_mode_command_topic         = HP_NAME "/cmnd/mode";
const char* heatpump_temperature_command_topic  = HP_NAME "/cmnd/temp";
const char* heatpump_fan_mode_command_topic     = HP_NAME "/cmnd/fan";
const char* heatpump_swing_mode_command_topic   = HP_NAME "/cmnd/vane";
const char* heatpump_debug_topic                = HP_NAME "/debug";
const char* ha_status_topic                     = "homeassistant/status"; // HA birth message, triggers discovery re-publish

// Customization, can be overridden in secrets.h
#ifndef MIN_TEMP
  #define MIN_TEMP  16
#endif
#ifndef MAX_TEMP
  #define MAX_TEMP  31
#endif
#ifndef TEMP_STEP
  #define TEMP_STEP 1
#endif
const float min_temp                    = MIN_TEMP;  // Minimum temperature, check value from heatpump remote control
const float max_temp                    = MAX_TEMP;  // Maximum temperature, check value from heatpump remote control
const float temp_step                   = TEMP_STEP; // Temperature setting step, check value from heatpump remote control
const char* mqtt_discov_prefix          = "homeassistant"; // Home Assistant MQTT Discovery Prefix

// fallback access point: opened when configured wifi is unreachable for this long
const char* ap_ssid                     = "MitsuCon-" HP_NAME; // open network, web interface at http://192.168.4.1
const unsigned long WIFI_AP_FALLBACK_MS = 60000;

// pinouts
const int redLedPin  = 0; // Onboard LED = digital pin 0 (red LED on adafruit ESP8266 huzzah)
const int blueLedPin = 2; // Onboard LED = digital pin 0 (blue LED on adafruit ESP8266 huzzah)

// sketch settings
const unsigned int SEND_ROOM_TEMP_INTERVAL_MS = 60000;
const unsigned int MQTT_RETRY_INTERVAL_MS     = 5000;
